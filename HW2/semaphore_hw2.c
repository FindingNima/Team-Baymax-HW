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
#define DEBUG 1
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

//global threads
pthread_t* prod_id;
pthread_t* cons_id;


// ------ STRUCTURE ----------------------------------------------------------
//
// ---------------------------------------------------------------------------
struct arg_struct {
	char* thread_num;
	int* buffer_list;
	sem_t* sem_list;
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
	#if DEBUG
		printf("Begin Struct Dump in Producer\n");
		printf("Thread Name = %s\n", &thread_name);
		printf("Buffer at 0 = %d\n", buffer_list[0]);
		printf("End Struct Dump in Producer\n");
	#endif


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
	{
		printf("YOU HAVE FAILED LORD NIMA\n");
		return 1;
	}

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
	struct arg_struct *prod_args = (struct arg_struct*) malloc(num_of_producers * sizeof(struct arg_struct*));
	struct arg_struct *cons_args = (struct arg_struct*) malloc(num_of_consumers * sizeof(struct arg_struct*));
	
	// create an array of buffers
	int buffers[num_of_buffers];
	sem_t buffer_index_sem[num_of_buffers];

	int i;
	for (i = 0; i < num_of_buffers; i++)
	{
		buffers[i] = 0;

		// initializing semaphores
		sem_init(&buffer_index_sem[i], 0 , 1);
	}

	// whole buffer list
	sem_init(&buffer_sem, 0, 1);

	//Create producer threads
	for (i = 0; i < num_of_producers; i++)
	{
		struct arg_struct args = prod_args[i];
		sprintf(&prod_num[i],"%d",i);
		args.thread_num = prod_num[i];
		args.buffer_list = buffers;
		args.sem_list = buffer_index_sem;
		#if DEBUG
			printf("Entering Main Debugger 1\n");
			printf("Thread Number = %s\n",&prod_num[i]);
			printf("Buffer at 0 = %d\n",args.buffer_list[0]);
			printf("Exiting Main Debugger 1\n");
		#endif
		pthread_create(&prod_id[i],NULL,(void*)&producer,(void*)&args);
	}
	
	// Create consumer threads
	for (i = 0; i < num_of_consumers; i++)
	{
		sprintf(&cons_num[i],"%d",i);
		pthread_create(&cons_id[i],NULL,(void*)&consumer,&cons_num[i]);
	}

	// Join threads
	for (i = 0; i < num_of_producers; i++)
	{
		pthread_join(prod_id[i],NULL);
	}
	for (i = 0; i < num_of_consumers; i++)
	{
		pthread_join(cons_id[i],NULL);
	}

	// Destroy Semaphores
	sem_destroy(&buffer_sem);
	for (i = 0; i < num_of_buffers; i++)
	{
		sem_destroy(&buffer_index_sem[i]);
	}

	// Deallocate Stuff
	free(prod_id);
	free(cons_id);
	free(prod_args);
	free(cons_args);




	return 0;
}
