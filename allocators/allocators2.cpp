#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

struct block
{
	char* data;
	int size;
	block* next = NULL;
};

block* free_blocks = NULL;
block* used_blocks = NULL;

char* memory = NULL;
int memory_size = 0;

struct header
{
	int size;
};

struct secret_header
{
	int magic_value;
};

char* writeHeader (char* headerAddress, int size)
{
	header* typedHeader = (header*) headerAddress;
	typedHeader->size = size;
	char* next_free = headerAddress + sizeof(header);
	return next_free;
}

char* readHeader (char* headerAddress)
{
	header* typedHeader = (header*) headerAddress;
	cout << typedHeader->size << " ";
	char* next_to_read = headerAddress + sizeof(header);
	return next_to_read;
}

char* writeSecretHeader (char* secretHeaderAddress, int magic_value)
{
	secret_header* typedSecretHeader = (secret_header*) secretHeaderAddress;
	typedSecretHeader->magic_value = magic_value;
	char* next_free = secretHeaderAddress + sizeof(secret_header);
	return next_free;
}

char* readSecretHeader (char* secretHeaderAddress)
{
	secret_header* typedSecretHeader = (secret_header*) secretHeaderAddress;
	cout << typedSecretHeader->magic_value << " ";
	char* next_to_read = secretHeaderAddress + sizeof(secret_header);
	return next_to_read;
}

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

char* allocate (int requested_size, char data)
{
	int allocated_size = requested_size + sizeof(header) + sizeof(secret_header);

	if(free_blocks->size < allocated_size)
	{
		cout << "ERROR: not enough memory" << endl;
		return NULL;
	}

	char* headerAddress = free_blocks->data;
	char* secretHeaderAddress = writeHeader(headerAddress, requested_size);
	char* dataAddress = writeSecretHeader(secretHeaderAddress, requested_size + 1);
	writeData(dataAddress, requested_size, data);

	cout << "allocated : " << allocated_size << endl;
	return dataAddress;
}

void displayUsedMemoryList ()
{
	cout << "Used Memory List : " << endl;
	block* used_block = used_blocks;
	while(used_block != NULL)
	{
		cout << "    (" << used_block->size << ") ";
		char* headerAddress = used_block->data;
		header* typedHeader = (header*) headerAddress;
		char* secretHeaderAddress = readHeader(headerAddress);
		char* dataAddress = readSecretHeader(secretHeaderAddress);
		readData(dataAddress, typedHeader->size);

		used_block = used_block->next;
		cout << endl;
	}
}

void displayFreeMemoryList ()
{
	cout << "Free Memory List : " << endl;
	block* free_block = free_blocks;
	while(free_block != NULL)
	{
		cout << "    (" << free_block->size << ") ";
		free_block = free_block->next;
		cout << endl;
	}
}

int main( int argc, char *argv[] )
{
	memory_size = 32 * sizeof(char);
	memory = (char*) malloc(memory_size);

	block first_block;
	first_block.data = memory;
	first_block.next = NULL;
	first_block.size = memory_size;

	free_blocks = &first_block;

	int dataASize = 6;
	char* userPointerA = allocate(dataASize, 'a');

	block blockA;
	if(userPointerA != NULL)
	{
		blockA.size = dataASize + sizeof(header) + sizeof(secret_header);
		blockA.data = free_blocks->data;
		blockA.next = NULL;

		if(used_blocks == NULL)
		{
			used_blocks = &blockA;
		}
		else
		{
			used_blocks->next = &blockA;
		}
		
		free_blocks->size -= dataASize + sizeof(header) + sizeof(secret_header);
		free_blocks->data += dataASize + sizeof(header) + sizeof(secret_header);
	}

	int dataBSize = 2;
	char* userPointerB = allocate(dataBSize, 'b');

	if(userPointerB != NULL)
	{
		block blockB;

		blockB.size = dataBSize + sizeof(header) + sizeof(secret_header);
		blockB.data = free_blocks->data;
		blockB.next = NULL;

		if(used_blocks == NULL)
		{
			used_blocks = &blockB;
		}
		else
		{
			used_blocks->next = &blockB;
		}

		free_blocks->size -= dataBSize + sizeof(header) + sizeof(secret_header);
		free_blocks->data += dataBSize + sizeof(header) + sizeof(secret_header);
	}

	displayUsedMemoryList();
	displayFreeMemoryList();

	free(memory);	
	return 0;
}
