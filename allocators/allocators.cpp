#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

char* blocks = NULL;
char* free_blocks = NULL;
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

void displayMemory ()
{
	for(int i = 0; i < memory_size; ++i)
	{
		cout << *(blocks + i);
	}
	cout << endl;
}

char* displayPopoPointer (char* popoPointerAddress)
{
	readHeader(popoPointerAddress);
	header* typedHeader = (header*) popoPointerAddress;

	char* secretHeaderAddress = popoPointerAddress + sizeof(header);

	for(int i = 0; i < (int) sizeof(header); ++i)
	{
		cout << *(secretHeaderAddress + i);
	}

	//secret_header* typedSecretHeader = (secret_header*) secretHeaderAddress;

	char* dataAddress = secretHeaderAddress + sizeof(secret_header);

	int i = 0;
	for(; i < typedHeader->size; ++i)
	{
		cout << *(dataAddress + i);
	}
	cout << endl;

	return dataAddress + i;
}

char* allocate (int requested_size)
{
	int allocated_size = requested_size + sizeof(header) + sizeof(secret_header);
	char* current_pointer = NULL;

	// find header and write h in it
	char* allocated_header = free_blocks;
	current_pointer = allocated_header;
	writeHeader(allocated_header, requested_size);
	/*for(; current_pointer != allocated_header + sizeof(header); ++current_pointer)
	{
		*current_pointer = 'h';
	}*/

	char* allocated_secret_header = free_blocks + sizeof(header);
	current_pointer = allocated_secret_header;
	for(; current_pointer != allocated_secret_header + sizeof(secret_header); ++current_pointer)
	{
		*current_pointer = 's';
	}

	char* pointer = free_blocks + sizeof(header) + sizeof(secret_header);
	current_pointer = pointer;
	for(; current_pointer != pointer + requested_size; ++current_pointer)
	{
		*current_pointer = 'm';
	}

	free_blocks = free_blocks + allocated_size;

	return pointer;
}

int main( int argc, char *argv[] )
{
	memory_size = 32 * sizeof(char);
	blocks = (char*) malloc(memory_size);
	free_blocks = blocks;

	for(int i = 0; i < memory_size; ++i)
	{
		*(blocks + i) = 'f';
	}
	displayMemory();
	allocate(2);
	allocate(6);

	/*bool quit = false;
	string answer = "";
	while(!quit)
	{
	    cout << "\n What is your full name? \t";
	    cin >> answer;
	    cout << "\n What is your current salary? \t";
	    cin >> answer;
	    cout << "\n What is your pay increase? \t";
	    cin >> answer;
	    cout << "\n your answer is : \t" + answer << endl;
	    quit = true;
	}*/
	displayMemory ();
	free(blocks);
	//std::cout << "Hello World" << std::endl;	
	return 0;
}
