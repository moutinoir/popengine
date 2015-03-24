#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

struct block
{
	char* data;
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

void writeHeader (char* headerAddress, int size)
{
	header* typedHeader = (header*) headerAddress;
	typedHeader->size = size;
}

void readHeader (char* headerAddress)
{
	header* typedHeader = (header*) headerAddress;
	cout << typedHeader->size;
}

void writeSecretHeader (char* secretHeaderAddress, int magic_value)
{
	secret_header* typedSecretHeader = (secret_header*) secretHeaderAddress;
	typedSecretHeader->magic_value = magic_value;
}

void readSecretHeader (char* secretHeaderAddress)
{
	secret_header* typedSecretHeader = (secret_header*) secretHeaderAddress;
	cout << typedSecretHeader->magic_value;
}

char* allocate (int requested_size)
{
	int allocated_size = requested_size + sizeof(header) + sizeof(secret_header);
	cout << allocated_size;
	return NULL;
}

int main( int argc, char *argv[] )
{
	memory_size = 32 * sizeof(char);
	memory = (char*) malloc(memory_size);

	free(memory);	
	return 0;
}
