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

void readUsedMemory ()
{
	cout << "Used Memory" << endl;
	header* memory_unit = used_memory;

	while(memory_unit != NULL)
	{
		cout << "    " << memory_unit->size << " " << memory_unit->magic_allocator_id << " ";
		readData((char*) memory_unit + sizeof(header), memory_unit->size);
		cout << endl;

		memory_unit = memory_unit->next;
	}
}

char* basicAllocate (int requested_size)
{
	int size = requested_size + sizeof(header);

	if(free_memory == NULL || free_memory->size < size)
	{
		cerr << "PopEngine Error: not enough free memory as one block" << endl;
		return NULL;
	}

	header* allocated_memory = free_memory;
	header* new_free_memory = (header*) free_memory + size;

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
		header* previous_used_memory = used_memory;
		while(previous_used_memory->next != NULL)
		{
			previous_used_memory = previous_used_memory->next;
		}
		previous_used_memory->next = allocated_memory;
	}

	return (char*) allocated_memory + sizeof(header);
}

int main( int argc, char *argv[] )
{
	memory_size = 128 * sizeof(char);
	if(memory_size < sizeof(header))
	{
		cerr << "PopEngine Error: not enough memory to create a header" << endl;
		return 1;
	}

	memory = (char*) malloc(memory_size);

	// initialize free_memory
	free_memory = (header*) memory;
	free_memory->size = memory_size - sizeof(header);
	free_memory->magic_allocator_id = 0;
	free_memory->next = NULL;

	char* new_memory = NULL;

	new_memory = basicAllocate(6);
	writeData(new_memory, 6, 'a');

	new_memory = basicAllocate(2);
	writeData(new_memory, 2, 'b');

	new_memory = basicAllocate(10);
	writeData(new_memory, 10, 'c');

	readUsedMemory();

	free(memory);	
	return 0;
}
