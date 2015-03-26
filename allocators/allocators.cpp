#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

char* memory = NULL;
int memory_size = 0;
int basicAllocatorMagicValue = 6548;

struct header
{
	int size;
	int magic_allocator_id;
	header* next;
};

header* free_memory = NULL;
header* used_memory = NULL;

void writeData (char* dataAddress, int size, char data)
{
	for(int i = 0; i < size; ++i)
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
		cout << "                " << memory_unit << " (" << memory_unit->size + sizeof(header) << ") " << memory_unit->size << " " << memory_unit->magic_allocator_id << " " << (void*)(((char*) memory_unit) + sizeof(header)) << " ";
		readData(((char*) memory_unit) + sizeof(header), memory_unit->size);
		cout << endl;

		memory_unit = memory_unit->next;
	}
}

char* basicAllocate (int requested_size)
{
	int size = requested_size + sizeof(header);

	if(free_memory == NULL)
	{
		cerr << "PopEngine Error: no free memory " << endl;
		return NULL;
	}

	header* previous_potential_memory = NULL;
	header* potential_memory = free_memory;

	while(potential_memory != NULL && potential_memory->size < size)
	{
		previous_potential_memory = potential_memory;
		potential_memory = potential_memory->next;
	}

	if(potential_memory == NULL)
	{
		cerr << "PopEngine Error: not enough free memory as one block to allocate " << requested_size << endl;
		return NULL;
	}

	if(potential_memory->size < size)
	{
		// never supposed to reach this code but just in case
		cerr << "PopEngine Error: not enough free memory as one block to allocate, this code shouldn't be reached " << requested_size << endl;
		return NULL;
	}

	// we found a block with enough memory
	// get allocated header
	header* allocated_memory = potential_memory;
	// find remaining free memory new header address
	header* remaining_free_memory = (header*)((char*) allocated_memory + size);

	// change remaining free memory header information
	remaining_free_memory->size = allocated_memory->size - size;
	remaining_free_memory->magic_allocator_id = 0;
	remaining_free_memory->next = allocated_memory->next;

	// update previous potential memory next pointer
	if(previous_potential_memory != NULL)
	{
		previous_potential_memory->next = remaining_free_memory;
	}
	else
	{
		free_memory = remaining_free_memory;
	}
	
	// set allocated memory information
	allocated_memory->size = requested_size;
	allocated_memory->magic_allocator_id = basicAllocatorMagicValue;
	allocated_memory->next = NULL;

	if(used_memory == NULL)
	{
		used_memory = allocated_memory;
	}
	else
	{
		allocated_memory->next = used_memory;
		used_memory = allocated_memory;
	}

	char* user_pointer = ((char*) allocated_memory) + sizeof(header);
	cout << "PopEngine Info: allocated " << requested_size << " at header address " <<  allocated_memory << " and user_pointer address " << (void*) user_pointer << endl;
	return user_pointer;
}

void basicFree (char* user_pointer)
{
	header* header_to_free = (header*) (user_pointer - sizeof(header));

	// check the magic number and the allocator id at the same time
	if(header_to_free->magic_allocator_id != basicAllocatorMagicValue)
	{
		cerr << "PopEngine Error: invalid header at address " << header_to_free << " user_pointer " << (void*) user_pointer << " magic_allocator_id is " << header_to_free->magic_allocator_id << " size is " << header_to_free->size << endl;
		return; 
	}
	header_to_free->magic_allocator_id = 0;

	// look for previous header in used memory
	if(header_to_free != used_memory)
	{
		header* previous_header = used_memory;
		while(previous_header->next != header_to_free && previous_header->next != NULL)
		{
			previous_header = previous_header->next;
		}

		if(previous_header->next == NULL)
		{
			cerr << "PopEngine Error: pointer wasn't in used memory " << &user_pointer << endl;
			return; 
		}

		// vanish the header from used memory
		if(previous_header->next == header_to_free)
		{
			previous_header->next = header_to_free->next;
		}
	}
	else
	{
		// vanish the header from used memory
		used_memory = header_to_free->next;
	}
	header_to_free->next = NULL;

	// add the pointer to free memory
	cout << "PopEngine Info: freed pointer of size " << header_to_free->size << " " << (void*) user_pointer << endl;

	// look for merges
	header* potential_mergeable_free_memory = free_memory;

	// look for a merge after mergeable memory
	bool is_merged_after_potential_free_memory = false;
	bool is_merged_before_potential_free_memory = false;
	while(potential_mergeable_free_memory != NULL && !is_merged_after_potential_free_memory)
	{
		//cout << "                Is potential_mergeable_free_memory block following address " << (void*) ((char*) potential_mergeable_free_memory + potential_mergeable_free_memory->size + sizeof(header)) ;
		//cout << " header_to_free address " << (void*) header_to_free << " ?" << endl;
		// potential_mergeable_free_memory block following address is header_to_free address
		if((char*) potential_mergeable_free_memory + potential_mergeable_free_memory->size + sizeof(header) == (char*) header_to_free )
		{
			// add memory after potential mergeable memory
			potential_mergeable_free_memory->size += header_to_free->size + sizeof(header);
			is_merged_after_potential_free_memory = true;

			cout << "PopEngine Info: merge freed header address" << (void*) header_to_free << " after free header address ";
			cout << (void*) potential_mergeable_free_memory << " which last block address is " << (void*) ((char*) potential_mergeable_free_memory + potential_mergeable_free_memory->size + sizeof(header) - 1) << endl;

			// try to merge with next free memory block (free memory list has to be ordered)
			// potential free memory has a next  
			// and potential free memory next header address is the address just after header to free block
			if(potential_mergeable_free_memory->next != NULL 
				&& (char*) potential_mergeable_free_memory->next == (char*) header_to_free + header_to_free->size + sizeof(header))
			{
				header* potential_mergeable_free_memory_next_next = potential_mergeable_free_memory->next->next;
				potential_mergeable_free_memory->size += potential_mergeable_free_memory->next->size + sizeof(header);
				potential_mergeable_free_memory->next = potential_mergeable_free_memory_next_next;
				is_merged_before_potential_free_memory = true;

				cout << "PopEngine Info: combo merge freed header address" << (void*) header_to_free << " which last block address is "; 
				cout << (void*) ((char*) header_to_free + header_to_free->size + sizeof(header) - 1) << " before free header address " << (void*) potential_mergeable_free_memory->next << endl;
			}
		}

		potential_mergeable_free_memory = potential_mergeable_free_memory->next;
	}

	// reset potential_mergeable_free_memory and keep track of previous potential_mergeable_free_memory
	header* previous_potential_mergeable_free_memory = NULL;
	potential_mergeable_free_memory = free_memory;

	// look for a merge before mergeable memory
	while(potential_mergeable_free_memory != NULL && !is_merged_before_potential_free_memory)
	{
		//cout << "                Is potential_mergeable_free_memory block address " << (void*) potential_mergeable_free_memory ;
		//cout << " is header_to_free block following address " << (void*) (((char*) header_to_free) + header_to_free->size + sizeof(header)) << " ?" << endl;
		// potential free memory header address is the address just after header to free block
		if((char*) potential_mergeable_free_memory == (char*) header_to_free + header_to_free->size + sizeof(header))
		{
			header_to_free->size += potential_mergeable_free_memory->size + sizeof(header);
			// add memory before the first memory in the free list
			if(previous_potential_mergeable_free_memory == NULL)
			{
				free_memory = header_to_free;
			}
			else
			{
				previous_potential_mergeable_free_memory->next = header_to_free;
			}
			is_merged_before_potential_free_memory = true;

			cout << "PopEngine Info: merge freed header address" << (void*) header_to_free << " which last block address is "; 
			cout << (void*) ((char*) header_to_free + header_to_free->size + sizeof(header) - 1) << " before free header address " << (void*) potential_mergeable_free_memory << endl;
		}

		previous_potential_mergeable_free_memory = potential_mergeable_free_memory;
		potential_mergeable_free_memory = potential_mergeable_free_memory->next;
	}

	// in case we couldn't merge just add the memory at the top of free memory list
	// TODO : keep me in the right order!!!!!!
	if(!is_merged_before_potential_free_memory && !is_merged_after_potential_free_memory)
	{
		header* previous_memory_unit = NULL;
		header* memory_unit = free_memory;
		while(memory_unit != NULL && memory_unit < header_to_free)
		{
			previous_memory_unit = memory_unit;
			memory_unit = memory_unit->next;
		}

		if(previous_memory_unit != NULL)
		{
			previous_memory_unit->next = header_to_free;
			header_to_free->next = memory_unit;
			cout << "PopEngine Info: put freed memory of size " << header_to_free->size << " and address " << (void*) user_pointer << " after " << (void*) previous_memory_unit << endl;
		}
		else
		{
			header_to_free->next = free_memory;
			free_memory = header_to_free;
			cout << "PopEngine Info: put freed memory of size " << header_to_free->size << " and address " << (void*) user_pointer << " on top of the free list" << endl;
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
	free_memory->next = NULL;

	char* memory_a = basicAllocate(6);
	if(memory_a != NULL)
		writeData(memory_a, 6, 'a');

	char* memory_b = basicAllocate(2);
	if(memory_b != NULL)
		writeData(memory_b, 2, 'b');

	char* memory_c = basicAllocate(1);
	if(memory_c != NULL)
		writeData(memory_c, 1, 'c');

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
