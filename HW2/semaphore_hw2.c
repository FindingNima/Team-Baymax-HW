// ----------------------------------------------------------------------------
// Semaphore - Producer / Consumer Problem
// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define TRUE 1
#define FALSE 0
#define DEBUG 0
#define BUFFER_SIZE 1024

// prototypes
void producer(void*);
void consumer(void*);
void printBuffer();

// ------ PRODUCER -----------------------------------------------------------
//
// ---------------------------------------------------------------------------
void producer(void* prod_num
{

}

// ------ CONSUMER -----------------------------------------------------------
//
// ---------------------------------------------------------------------------
void consumer(void* cons_num)
{
	
}

// ------ PRINT BUFFER -------------------------------------------------------
//
// ---------------------------------------------------------------------------
void printBuffer()
{
	
}


// ------ MAIN ---------------------------------------------------------------
//
// ---------------------------------------------------------------------------
int main(int argc, char const *argv[])
{
	if (argc < 5 || argc > 5)  
		return 1;

	// intializing the 4 arguments passed in from the shell
	num_of_producers = atoi(argv[1]);
	num_of_consumers = atoi(argv[2]);
	num_of_buffers = atoi(argv[3]);
	num_of_items = atoi(argv[4]);


	return 0;
}