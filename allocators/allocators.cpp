#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

char* memory = NULL;
int memory_size = 0;
int basicAllocatorMagicValue = 6548;
int word_size_power_of_2 = 3;
int word_size = 8;
int block_size = 8;

struct header
{
	int size;
	int magic_allocator_id;
	header* next;
	header* previous;
	header* global_next;
	header* global_previous;
	int padding;
};

header* free_memory = NULL;
header* used_memory = NULL;

void writeData (char* dataAddress, char data)
{
	header* header_info = (header*)(dataAddress - sizeof(header));
	
	for(int i = 0; i < header_info->size; ++i)
	{
		*(dataAddress + i) = data;
	}
}

void readData (char* dataAddress, int size)
{
	for(int i = 0; i < size; ++i)
	{
		cout << *(dataAddress + i);
	}
	cout << " ";
}

void readMemoryList (header* memory_list, string memory_list_name)
{
	cout << "PopEngine Info: " << memory_list_name << endl;
	header* memory_unit = memory_list;

	while(memory_unit != NULL)
	{
		cout << "                     " << (void*) memory_unit << " (" << memory_unit->size + sizeof(header) << ") " << memory_unit->size << " " << memory_unit->padding << " " << memory_unit->magic_allocator_id << " " << (void*)(((char*) memory_unit) + sizeof(header)) << " ";
		readData(((char*) memory_unit) + sizeof(header), memory_unit->size);
		cout << endl;

		memory_unit = memory_unit->next;
	}
}

char* basicAllocate (int requested_size)
{
	cout << "PopEngine Info: Allocate  " << requested_size;
	// check if requested_size is a multiple of the block size
	/*int remainder = requested_size % block_size;
	int quotient = requested_size / block_size;

	if(remainder != 0)
	{
		requested_size = (quotient + 1) * block_size;
	}*/

	// allocate enough memory for the data, the header, and a margin to align the user pointer
	int allocate_size = requested_size + sizeof(header) + word_size;
	cout << " (behind the scene : " << allocate_size << ")" << endl;

	if(free_memory == NULL)
	{
		cerr << "PopEngine Error: no free memory " << endl;
		return NULL;
	}

	//header* previous_memory = NULL;
	header* memory = free_memory;

	while(memory != NULL && memory->size < allocate_size)
	{
		//previous_memory = memory;
		memory = memory->next;
	}

	if(memory == NULL)
	{
		cerr << "PopEngine Error: not enough free memory as one block to allocate " << requested_size << endl;
		return NULL;
	}

	if(memory->size < allocate_size)
	{
		// never supposed to reach this code but just in case
		cerr << "PopEngine Error: not enough free memory as one block to allocate, this code shouldn't be reached " << requested_size << endl;
		return NULL;
	}

	// we found a block with enough memory
	cout << "                free memory : " << (void*) memory << " size : " << memory->size << endl;

	// align user pointer
	char* user_pointer = ((char*) memory) + sizeof(header) + word_size;
	char* aligned_user_pointer = (char*)((uintptr_t)user_pointer & ~(uintptr_t)(word_size - 1));
	cout << "                user pointer : " << (void*) user_pointer << " aligned pointer : " << (void*) aligned_user_pointer << endl;

	// get padding
	int difference = (int)((uintptr_t)user_pointer - (uintptr_t)aligned_user_pointer);
	if(difference == 0)
	{
		difference = word_size;
	}
	int padding = word_size - difference;

	user_pointer = user_pointer - word_size + padding;

	cout << "                Padding : " << padding << " User pointer : " << (void*) user_pointer;

	// save memory data to add the padding
	int memory_size = memory->size;
	header* memory_next = memory->next;
	header* memory_previous = memory->previous;

	// get allocated header
	header* allocated_memory = (header*)((char*) user_pointer - sizeof(header));
	cout << "  Header pointer : " << (void*) allocated_memory << endl;

	// set allocated memory information
	allocated_memory->size = requested_size;
	allocated_memory->padding = padding;
	allocated_memory->magic_allocator_id = basicAllocatorMagicValue;
	allocated_memory->next = NULL;
	allocated_memory->previous = NULL;
	
	// find remaining free memory new header address
	header* remaining_memory = (header*)((char*) user_pointer + allocated_memory->size);

	// change remaining free memory header information
	remaining_memory->size = memory_size - allocated_memory->size - padding - sizeof(header);
	remaining_memory->magic_allocator_id = 0;
	remaining_memory->next = memory_next;
	remaining_memory->previous = memory_previous;

	// update previous memory next pointer
	if(memory_previous != NULL)
	{
		memory_previous->next = remaining_memory;
	}
	else
	{
		free_memory = remaining_memory;
	}
	
	// add allocated header to used memory
	if(used_memory == NULL)
	{
		used_memory = allocated_memory;
	}
	else
	{
		allocated_memory->next = used_memory;
		used_memory = allocated_memory;
		allocated_memory->next->previous = allocated_memory;
	}

	return user_pointer;
}

void basicFree (char* user_pointer)
{
	cout << "PopEngine Info: Free User Pointer " << (void*) user_pointer << endl;
	header* header_to_free = (header*) (user_pointer - sizeof(header));

	// check the magic number and the allocator id at the same time
	if(header_to_free->magic_allocator_id != basicAllocatorMagicValue)
	{
		cerr << "PopEngine Error: invalid header at address " << header_to_free << " user_pointer " << (void*) user_pointer << " magic_allocator_id is " << header_to_free->magic_allocator_id << " size is " << header_to_free->size << endl;
		return; 
	}
	header_to_free->magic_allocator_id = 0;

	// remove header from used memory
	header* previous = header_to_free->previous;
	header* next = header_to_free->next;

	if(previous != NULL)
	{
		previous->next = next;
	}

	if(next != NULL)
	{
		next->previous = previous;
	}

	if(header_to_free == used_memory)
	{
		used_memory = next;
	}

	// add the pointer to free memory
	cout << "                Free Header " << (void*) header_to_free << " Size " << header_to_free->size << " Padding " << header_to_free->padding << endl;

	// move back the header is case there was some padding
	if(header_to_free->padding > 0)
	{
		int header_to_free_size = header_to_free->size;
		header* header_to_free_next = header_to_free->next;
		int header_to_free_padding = header_to_free->padding;

		header_to_free = (header*)((char*) header_to_free - header_to_free->padding);

		header_to_free->size = header_to_free_size + header_to_free_padding;
		header_to_free->next = header_to_free_next;
		header_to_free->padding = 0;
		header_to_free->magic_allocator_id = 0;
	}

	// look for merges
	header* mergeable_memory = NULL;
	header* mergeable_memory_next = free_memory;

	// look for a place to put memory
	bool is_merged_after_memory = false;
	bool is_merged_before_memory = false;
	bool is_tidied = false;

	while((mergeable_memory != NULL || mergeable_memory_next != NULL) && !is_merged_after_memory && !is_merged_before_memory && !is_tidied)
	{
		// look for a merge after
		if(mergeable_memory != NULL && (char*) mergeable_memory + mergeable_memory->size + sizeof(header) == (char*) header_to_free - header_to_free->padding)
		{
			cout << "                Merged after Memory : " << (void*) mergeable_memory << " --> " << (void*) ((char*) mergeable_memory + sizeof(header) + mergeable_memory->size) << " FUSION "; 
			cout << (void*) ((char*)header_to_free - header_to_free->padding) << " <-- To Free : " << (void*) header_to_free << endl;

			// add memory after potential mergeable memory
			mergeable_memory->size += header_to_free->size + sizeof(header) + header_to_free->padding;
			is_merged_after_memory = true;

			header_to_free = mergeable_memory;
		}

		// look for a merge before next
		if(mergeable_memory_next != NULL 
				&& (char*) mergeable_memory_next == (char*) header_to_free + header_to_free->size + sizeof(header))
		{
			cout << "                Merge after Header : " << (void*) header_to_free << " --> " << (void*) ((char*) header_to_free + header_to_free->size + sizeof(header)) << " FUSION "; 
			cout << (void*) mergeable_memory_next << " (Memory) " << endl;

			header* mergeable_memory_next_next = mergeable_memory_next->next;
			header_to_free->size += mergeable_memory_next->size + sizeof(header);
			header_to_free->next = mergeable_memory_next_next;

			if(!is_merged_after_memory)
			{
				header_to_free->previous = mergeable_memory;
			}

			if(mergeable_memory == NULL)
			{
				free_memory = header_to_free;
			}

			is_merged_before_memory = true;
		}

		// put memory in between if it's appropriate
		if(!is_merged_before_memory && !is_merged_after_memory)
		{

			if((mergeable_memory != NULL && header_to_free > mergeable_memory && mergeable_memory_next == NULL)
				|| (mergeable_memory_next != NULL && header_to_free > mergeable_memory && header_to_free < mergeable_memory_next))
			{

				if(mergeable_memory != NULL)
				{
					cout << "                Put after " << (void*) mergeable_memory << endl;
					header_to_free->previous = mergeable_memory;
					mergeable_memory->next = header_to_free;
				}

				if(mergeable_memory_next != NULL)
				{
					cout << "                Put before " << (void*) mergeable_memory_next << endl;
					header_to_free->next = mergeable_memory_next;
					mergeable_memory_next->previous = header_to_free;

					if(mergeable_memory == NULL)
					{
						free_memory = header_to_free;
					}
				}

				is_tidied = true;
			}
		}

		// update current and next memory
		mergeable_memory = mergeable_memory_next;
		if(mergeable_memory != NULL)
		{
			mergeable_memory_next = mergeable_memory->next;
		}
		else
		{
			mergeable_memory_next = NULL;
		}
	}
}

int main( int argc, char *argv[] )
{
	memory_size = 256 * sizeof(char);

	if(memory_size < (int) sizeof(header))
	{
		cerr << "PopEngine Error: not enough memory to create a header" << endl;
		return 1;
	}

	cout << "PopEngine Info: memory size " << memory_size << endl;
	cout << "PopEngine Info: header size " << sizeof(header) << endl;
	memory = (char*) malloc(memory_size);

	// initialize free_memory
	free_memory = (header*) memory;
	free_memory->size = memory_size - sizeof(header);
	free_memory->magic_allocator_id = 0;
	free_memory->padding = 0;
	free_memory->next = NULL;
	free_memory->previous = NULL;

	readMemoryList(used_memory, "Used Memory");
	readMemoryList(free_memory, "Free Memory");

	char* memory_a = basicAllocate(6);
	if(memory_a != NULL)
		writeData(memory_a, 'a');

	char* memory_b = basicAllocate(2);
	if(memory_b != NULL)
		writeData(memory_b, 'b');

	char* memory_c = basicAllocate(1);
	if(memory_c != NULL)
		writeData(memory_c, 'c');

	readMemoryList(used_memory, "Used Memory");
	readMemoryList(free_memory, "Free Memory");

	if(memory_c != NULL)
	{
		basicFree(memory_c);
		memory_c = NULL;
	}

	readMemoryList(used_memory, "Used Memory");
	readMemoryList(free_memory, "Free Memory");

	if(memory_a != NULL)
	{
		basicFree(memory_a);
		memory_a = NULL;
	}

	readMemoryList(used_memory, "Used Memory");
	readMemoryList(free_memory, "Free Memory");

	if(memory_b != NULL)
	{
		basicFree(memory_b);
		memory_b = NULL;
	}

	readMemoryList(used_memory, "Used Memory");
	readMemoryList(free_memory, "Free Memory");

	/*char* memory_a = basicAllocate(6);
	if(memory_a != NULL)
		writeData(memory_a, 6, 'a');

	char* memory_b = basicAllocate(2);
	if(memory_b != NULL)
		writeData(memory_b, 2, 'b');

	readMemoryList(used_memory, "Used Memory");
	readMemoryList(free_memory, "Free Memory");

	char* memory_c = basicAllocate(190);
	if(memory_c != NULL)
		writeData(memory_c, 190, 'c');

	char* memory_d = basicAllocate(40);
	if(memory_d != NULL)
		writeData(memory_d, 40, 'd');

	readMemoryList(used_memory, "Used Memory");
	readMemoryList(free_memory, "Free Memory");

	if(memory_d != NULL)
	{
		basicFree(memory_d);
		memory_d = NULL;
	}

	readMemoryList(used_memory, "Used Memory");
	readMemoryList(free_memory, "Free Memory");

	char* memory_e = basicAllocate(12);
	if(memory_e != NULL)
		writeData(memory_e, 12, 'e');

	readMemoryList(used_memory, "Used Memory");
	readMemoryList(free_memory, "Free Memory");

	char* memory_f = basicAllocate(3);
	if(memory_f != NULL)
		writeData(memory_f, 3, 'f');

	readMemoryList(used_memory, "Used Memory");
	readMemoryList(free_memory, "Free Memory");

	if(memory_a != NULL)
	{
		basicFree(memory_a);
		memory_a = NULL;
	}

	readMemoryList(used_memory, "Used Memory");
	readMemoryList(free_memory, "Free Memory");

	char* memory_g = basicAllocate(109);
	if(memory_g != NULL)
		writeData(memory_g, 109, 'g');

	readMemoryList(used_memory, "Used Memory");
	readMemoryList(free_memory, "Free Memory");*/

	free(memory);	
	return 0;
}
