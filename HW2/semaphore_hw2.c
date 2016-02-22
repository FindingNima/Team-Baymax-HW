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
int num_of_items;

//Global Buffer List
int* buffer_list;
sem_t* index_sem_list;

int total_produced = 0;
int total_consumed = 0;

// global semaphores
sem_t buffer_sem;

//global threads
pthread_t* prod_id;
pthread_t* cons_id;


// ------ STRUCTURE ----------------------------------------------------------
//
// ---------------------------------------------------------------------------
struct arg_struct {
	int thread_num;
	//int* buffer_list;
	//sem_t* index_sem_list;
};

// ------ PRODUCER -----------------------------------------------------------
//
// ---------------------------------------------------------------------------
void producer(void* args)
{
	struct arg_struct *arguments = (struct arg_struct*) args;
	#if DEBUG
	printf("Enter Producer %d\n",arguments -> thread_num);
	#endif
	//char* thread_name = &arguments -> thread_num;
	int thread_num = arguments -> thread_num;
	//int* buffer_list = arguments -> buffer_list;		// number in each buffer
	//sem_t* index_sem_list = arguments -> index_sem_list;			//

	#if DEBUG
		printf("Begin Struct Dump in Producer\n");
		printf("Thread Name = %d\n", thread_num);
		printf("Buffer at 0 = %d\n", buffer_list[0]);
		printf("End Struct Dump in Producer\n");
	#endif

	while(total_produced < num_of_items)
	{
		// spin lock bitch
		if (sem_trywait(&buffer_sem) == 0) 
		{
			// check for 1000 produced
			if (total_produced != 0 && total_produced % 1000 == 0 )
			{	
				printBuffer(total_produced, buffer_list);
			}

			int i;
			for(i = 0; i < num_of_buffers; i++)
			{
				if((buffer_list[i] < BUFFER_SIZE) && sem_trywait(&index_sem_list[i]) == 0)
				{
					total_produced++;
					buffer_list[i]++;
					sem_post(&index_sem_list[i]);
					break;
				}
				if (i == num_of_buffers - 1)
				{
					printf("Producer Thread %d Yielded\n",thread_num);
					pthread_yield();
				}
			}
			sem_post(&buffer_sem);
		}
	}
	printf("Thread %d Finished\n",thread_num);
}

// ------ CONSUMER -----------------------------------------------------------
//
// ---------------------------------------------------------------------------
void consumer(void* args)
{
	char* thread_num = (char*) args;	
	//struct arg_struct *arguments2 = (struct arg_struct*) args;
	#if DEBUG
	//printf("Enter Consumer %d\n",arguments -> thread_num);
	#endif
	//char* thread_name = &arguments -> thread_num;
	//int thread_num = 2;
	//sprintf(&thread_num,"%d",arguments2 -> thread_num);

	//printf("DID I SEGFAULT #</3\n");
	//int* buffer_list = arguments -> buffer_list; // number in each buffer
	//printf("DID I MOTHER FUCKING SEGFAULT?\n");
	//sem_t* index_sem_list = arguments -> index_sem_list;
	#if DEBUG
	printf("DID I SEGFAULT? #<3\n");
	#endif
	while(total_consumed < num_of_items)
	{
		// spin lock bitch
		if (sem_trywait(&buffer_sem) == 0) 
		{
		#if DEBUG
		printf("I GOT A SEMAPHORE!!!\n");	
		#endif
			int i;
			for(i = 0; i < num_of_buffers; i++)
			{
				#if DEBUG
				printf("ENTERED FOR LOOP\n");
				printf("BUFFER LIST: %d\n",buffer_list[i]);
				#endif
				if((buffer_list[i] > 0) && sem_trywait(&index_sem_list[i]) == 0)
				{
				#if DEBUG
				printf("ENTERED IF STATEMENT\n");
				#endif
					total_consumed++;
					buffer_list[i]--;
					sem_post(&index_sem_list[i]);
					break;
				}
				if (i == num_of_buffers - 1) 
				{
					printf("Consumer Thread %s is Yielding\n",thread_num);
					pthread_yield();
				}
			}
			sem_post(&buffer_sem);


		}
	}
	printf("Consumer Thread %s Finished\n",thread_num);
}

// ------ PRINT BUFFER -------------------------------------------------------
//
// ---------------------------------------------------------------------------
void printBuffer(int total_produced, int* buffer_list)
{	
	int i;
	for(i = 0; i < num_of_buffers; i++) 
	{
		printf("SharedBuffer %d has %d of items\n", i, buffer_list[i]);
	}

	printf("%d items created\n", total_produced);
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
	//char *prod_num[num_of_producers];
	char *cons_num[num_of_consumers];

	// allocate argument structures
	struct arg_struct *prod_args = (struct arg_struct*) malloc(num_of_producers * sizeof(struct arg_struct));
	struct arg_struct *cons_args = (struct arg_struct*) malloc(num_of_consumers * sizeof(struct arg_struct));
	
	// create an array of buffers
	buffer_list = (int*)malloc(num_of_buffers * sizeof(int*));
	index_sem_list = (sem_t*)malloc(num_of_buffers * sizeof(sem_t)); 
	//sem_t buffer_index_sem[num_of_buffers];

	int i;
	for (i = 0; i < num_of_buffers; i++)
	{
		buffer_list[i] = 0;

		// initializing semaphores
		sem_init(&index_sem_list[i], 0 , 1);
	}

	// whole buffer list
	sem_init(&buffer_sem, 0, 1);

	//Create producer threads
	for (i = 0; i < num_of_producers; i++)
	{
		struct arg_struct *args = &prod_args[i];
		//sprintf(&prod_num[i],"%d",i);
		args -> thread_num = i;
		//args -> buffer_list = buffers;
		//args -> index_sem_list = buffer_index_sem;
		#if DEBUG
			printf("Entering Main Debugger 1\n");
			printf("Thread Number = %d\n",i);
			//printf("Buffer at 0 = %d\n",args -> buffer_list[0]);
			printf("Exiting Main Debugger 1\n");
		#endif
		pthread_create(&prod_id[i],NULL,(void*)&producer,(void*)args);
	}
	// Create consumer threads
	for (i = 0; i < num_of_consumers; i++)
	{
		//struct arg_struct *args = &cons_args[i];
		sprintf(&cons_num[i],"%d",i);
		//args -> thread_num = i;
		//args -> buffer_list = buffers;
		//args -> index_sem_list = buffer_index_sem;
		//int* passi = &i;
		pthread_create(&cons_id[i],NULL,(void*)&consumer,(void*)&cons_num[i]);
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
		sem_destroy(&index_sem_list[i]);
	}
	// Deallocate Stuff
	free(prod_id);
	free(cons_id);
	//free(prod_args);
	//free(cons_args);




	return 0;
}
