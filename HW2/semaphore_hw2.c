// ----------------------------------------------------------------------------
// Semaphore - Producer / Consumer Problem
// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#define TRUE 1
#define FALSE 0
#define DEBUG 0
#define BUFFER_SIZE 1024

// prototypes
void producer(void*);
void consumer(void*);
void printBuffer();

// global variables
int num_of_producers;
int num_of_consumers;
int num_of_buffers;
int	num_of_items;

// global semaphores
sem_t buffer_sem;

// ------ STRUCTURE ----------------------------------------------------------
//
// ---------------------------------------------------------------------------
struct args_struct {
	char* thread_num;
	int* buffer_list;
	sem_t sem_list;
};
// ------ PRODUCER -----------------------------------------------------------
//
// ---------------------------------------------------------------------------
void producer(void* args)
{
	struct arg_struct *arguments = (struct arg_struct*) args;
	char* thread_name = arguments -> thread_num;
	int* buffer_list = arguments -> buffer_list;
	sem_t* sem_list = arguments -> sem_list;

}

// ------ CONSUMER -----------------------------------------------------------
//
// ---------------------------------------------------------------------------
void consumer(void* args)
{
	struct arg_struct *arguments = (struct arg_struct*) args;
	char* thread_name = arguments -> thread_num;
	int* buffer_list = arguments -> buffer_list;
	sem_t* sem_list = arguments -> sem_list;
	
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

	// allocate threads
	prod_id = (pthread_t*) malloc(num_of_producers * sizeof(pthread_t*));
	cons_id = (pthread_t*) malloc(num_of_consumers * sizeof(pthread_t*));
	char *prod_num[num_of_producers];
	char *cons_num[num_of_consumers];

	// allocate argument structures
	struct *prod_args = (struct*) malloc(num_of_producers * sizeof(struct*));
	struct *cons_args = (struct*) malloc(num_of_consumers * sizeof(struct*));
	
	// create an array of buffers
	int buffers[num_of_buffers];
	sem_t buffer_index_sem[num_of_buffers];

	int i;
	for (i = 0; i < num_of_buffers; i++)
	{
		buffers[i] = 0;

		// initializing semaphores
		sem_int(&buffer_index_sem[i], 0 , 1);
	}

	// whole buffer list
	sem_init(&buffer_sem, 0, 1);

	//Create producer threads
	for (i = 0; i < num_of_producers; i++)
	{
		struct *args = *prod_args[i];
		sprintf(&prod_num[i],"%d",i);
		args.thread_num = prod_num[i];
		args.buffer_list = buffers[i];
		args.sem_list = buffer_index_sem[i];
		pthread_create(&prod_id[i],NULL,(void*)&producer,(void*)&args);
	}
	
	//Create consumer threads
	for (i = 0; i < num_of_consumers; i++)
	{
		sprintf(&cons_num[i],"%d",i);
		pthread_create(&cons_id[i],NULL,(void*)&consumer,&cons_num[i]);
	}


	return 0;
}
