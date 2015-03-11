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

void displayMemory ()
{
	for(int i = 0; i < memory_size; ++i)
	{
		cout << *(blocks + i);
	}
	cout << endl;
}

char* allocate (int requested_size)
{
	int allocated_size = requested_size + sizeof(header) + sizeof(secret_header);
	char* current_pointer = NULL;

	// find header and write h in it
	char* allocated_header = free_blocks;
	current_pointer = allocated_header;
	for(; current_pointer != allocated_header + sizeof(header); ++current_pointer)
	{
		*current_pointer = 'h';
	}

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
