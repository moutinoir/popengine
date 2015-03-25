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
		cout << "        " << memory_unit << " (" << memory_unit->size + sizeof(header) << ") " << memory_unit->size << " " << memory_unit->magic_allocator_id << " " << (void*)(((char*) memory_unit) + sizeof(header)) << " ";
		readData(((char*) memory_unit) + sizeof(header), memory_unit->size);
		cout << endl;

		memory_unit = memory_unit->next;
	}
}

char* basicAllocate (int requested_size)
{
	int size = requested_size + sizeof(header);

	if(free_memory == NULL || free_memory->size < size)
	{
		cerr << "PopEngine Error: not enough free memory as one block to allocate " << requested_size << endl;
		return NULL;
	}

	header* allocated_memory = free_memory;
	header* new_free_memory = free_memory + size;

	new_free_memory->size = free_memory->size - size;
	new_free_memory->magic_allocator_id = 0;
	new_free_memory->next = free_memory->next;

	free_memory = new_free_memory;

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
	header* typed_header = (header*) (user_pointer - sizeof(header));

	// check the magic number and the allocator id at the same time
	if(typed_header->magic_allocator_id != basicAllocatorMagicValue)
	{
		cerr << "PopEngine Error: invalid header at address " << typed_header << " user_pointer " << (void*) user_pointer << " magic_allocator_id is " << typed_header->magic_allocator_id << " size is " << typed_header->size << endl;
		return; 
	}
	typed_header->magic_allocator_id = 0;

	// look for previous header in used memory
	if(typed_header != used_memory)
	{
		header* previous_header = used_memory;
		while(previous_header->next != typed_header && previous_header->next != NULL)
		{
			previous_header = previous_header->next;
		}

		if(previous_header->next == NULL)
		{
			cerr << "PopEngine Error: pointer wasn't in used memory " << &user_pointer << endl;
			return; 
		}

		// vanish the header from used memory
		if(previous_header->next == typed_header)
		{
			previous_header->next = typed_header->next;
		}
	}
	else
	{
		// vanish the header from used memory
		used_memory = typed_header->next;
	}
	typed_header->next = NULL;

	// add the pointer to free memory
	cout << "PopEngine Info: freed pointer of size " << typed_header->size << " " << (void*) user_pointer << endl;
	typed_header->next = free_memory;
	free_memory = typed_header;
}

int main( int argc, char *argv[] )
{
	memory_size = 128 * sizeof(char);

	if(memory_size < sizeof(header))
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

	char* memory_c = basicAllocate(120);
	if(memory_c != NULL)
		writeData(memory_c, 120, 'c');

	if(memory_b != NULL)
	{
		basicFree(memory_b);
		memory_b = NULL;
	}

	if(memory_a != NULL)
	{
		basicFree(memory_a);
		memory_a = NULL;
	}

	readMemoryList(used_memory, "Used Memory");
	readMemoryList(free_memory, "Free Memory");

	free(memory);	
	return 0;
}
