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

block* free_blocks_ = NULL;
block* used_blocks_ = NULL;

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

void readHeader (char* headerAddress)
{
	header* typedHeader = (header*) headerAddress;
	cout << typedHeader->size;
}

char* writeSecretHeader (char* secretHeaderAddress, int magic_value)
{
	secret_header* typedSecretHeader = (secret_header*) secretHeaderAddress;
	typedSecretHeader->magic_value = magic_value;
	char* next_free = secretHeaderAddress + sizeof(secret_header);
	return next_free;
}

void readSecretHeader (char* secretHeaderAddress)
{
	secret_header* typedSecretHeader = (secret_header*) secretHeaderAddress;
	cout << typedSecretHeader->magic_value;
}

char* allocate (int requested_size)
{
	int allocated_size = requested_size + sizeof(header) + sizeof(secret_header);

	if(free_blocks_->size < allocated_size)
	{
		cout << "ERROR: not enough memory" << endl;
		return NULL;
	}

	char* headerAddress = free_blocks_->data;
	char* secretHeaderAddress = writeHeader(headerAddress, requested_size);
	char* dataAddress = writeSecretHeader(secretHeaderAddress, requested_size);

	cout << "allocated : " << allocated_size << endl;
	return dataAddress;
}

int main( int argc, char *argv[] )
{
	memory_size = 32 * sizeof(char);
	memory = (char*) malloc(memory_size);

	block first_block;
	first_block.data = memory;
	first_block.next = NULL;
	first_block.size = memory_size;

	free_blocks_ = &first_block;

	int dataASize = 6;
	char* userPointerA = allocate(dataASize);

	block blockA;
	if(userPointerA != NULL)
	{
		blockA.size = dataASize + sizeof(header) + sizeof(secret_header);
		blockA.data = free_blocks_->data;
		blockA.next = NULL;

		if(used_blocks_ == NULL)
		{
			used_blocks_ = &blockA;
		}
		else
		{
			used_blocks_->next = &blockA;
		}
		
		free_blocks_->size -= dataASize + sizeof(header) + sizeof(secret_header);
		free_blocks_->data += dataASize + sizeof(header) + sizeof(secret_header);
	}

	int dataBSize = 2;
	char* userPointerB = allocate(dataBSize);

	if(userPointerB != NULL)
	{
		block blockB;

		blockB.size = dataBSize + sizeof(header) + sizeof(secret_header);
		blockB.data = free_blocks_->data;
		blockB.next = NULL;

		if(used_blocks_ == NULL)
		{
			used_blocks_ = &blockB;
		}
		else
		{
			used_blocks_->next = &blockB;
		}

		free_blocks_->size -= dataBSize + sizeof(header) + sizeof(secret_header);
		free_blocks_->data += dataBSize + sizeof(header) + sizeof(secret_header);
	}

	free(memory);	
	return 0;
}
