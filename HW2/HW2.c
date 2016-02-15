// ----------------------------------------------------------------------------
// HW2: Consumer Producer Problem with multiple buffers
// Authors: Emily Le, Max Wolotsky, Daniel
// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#define N 1024
#define TRUE 1
#define FALSE 0
#define DEBUG 1

// global variables
int num_of_producers;		// number of producers
int num_of_consumers;		// number of consumers
int num_of_buffers;			// number of buffers
int num_of_items;  			// number of items (will decrement)
int maximum;				// maximum = number of items (won't decrement)

sem_t *mutexes;				
sem_t *empties;
sem_t *fulls;
sem_t print_sem;			// print buffer semephore
pthread_t *prod_id;
pthread_t *cons_id;

// holds all the buffers
int **bufferList;			// holds all the buffers
int *buffer_idx;				// holds the number of items in each buffer

// prototypes
void bufferprinter(void);
void producer(void*);
void consumer(void*);
int produce_item(void);
void insert_item(int,int);
int remove_item(int);
void consume_item(int);

int main(int argc, char const *argv[])
{
	if (argc < 5 || argc == 0)  
		return 1;

	#ifdef DEBUG
	printf("Initializing Arguments\n");
	#endif

	// intializing the 4 arguments passed in from the shell
	num_of_producers = atoi(argv[1]);
	num_of_consumers = atoi(argv[2]);
	num_of_buffers = atoi(argv[3]);
	num_of_items = atoi(argv[4]);
	maximum = num_of_items;
	
	#ifdef DEBUG
	printf("Initializing Buffers\n");
	#endif

	//initialize buffers
	buffer_idx = (int *) malloc(num_of_buffers * sizeof(int*));
	bufferList = (int **) malloc(num_of_buffers * sizeof(int**));

	int j;
	for (j = 0; j < num_of_buffers;j++)
	{
		buffer_idx[j] = 0;

		int k;
		for (k = 0; k < N; k++)
		{
			*(bufferList + j) = (int*)malloc(N * sizeof(int*));
			*(*(bufferList + j) + k) = 0;
		}
	}

	#ifdef DEBUG
	printf("Allocating Space\n");
	#endif

	// allocating space for semaphores/threads for all shared buffers
	mutexes = (sem_t *) malloc(num_of_buffers * sizeof(sem_t*));  
	empties = (sem_t*) malloc(num_of_buffers * sizeof(sem_t*));
	fulls = (sem_t*) malloc(num_of_buffers * sizeof(sem_t*));
	prod_id = (pthread_t*) malloc(num_of_producers * sizeof(pthread_t*));
	cons_id = (pthread_t*) malloc(num_of_consumers * sizeof(pthread_t*));

	#ifdef DEBUG
	printf("Initializing Semaphores\n");
	#endif

	// initializing semaphores
	for (j = 0; j < num_of_buffers;j++)
	{
		sem_init(&mutexes[j],0,1);
		sem_init(&empties[j],0,N);
		sem_init(&fulls[j],0,0);
	}
	sem_init(&print_sem,0,1000);

	#ifdef DEBUG
	printf("Creating BufferPrinter Thread\n");
	#endif

	// create bufferprinter thread
	pthread_t printer_thread;
	pthread_create(&printer_thread,NULL,(void*)&bufferprinter,0);

	#ifdef DEBUG
	printf("Creating Producer and Consumer Threads\n");
	#endif
	
	char *prod_num[num_of_producers];
	char *cons_num[num_of_consumers];

	// creating producer and consumer threads
	int i;
	for (i = 0; i < num_of_producers; i++) 
	{
		sprintf(&prod_num[i],"%d",i);
		pthread_create(&prod_id[i],NULL,(void*)&producer,&prod_num[i]);
	}

	for (i = 0; i < num_of_consumers; i++) 
	{
		sprintf(&cons_num[i],"%d",i);
		pthread_create(&cons_id[i],NULL,(void*)&consumer,&cons_num[i]);
	}

	#ifdef DEBUG
	printf("Joining Threads\n");
	#endif

	// joining threads
	for (i = 0; i < num_of_producers; i++) 
	{
		pthread_join(prod_id[i],NULL);
	}

	for (i = 0; i < num_of_consumers; i++) 
	{
		pthread_join(cons_id[i],NULL);
	}

	#ifdef DEBUG
	printf("Destroy Semaphores\n");
	#endif

	// destroy semaphores
	for (j = 0; j < num_of_buffers;j++)
	{
		sem_destroy(&mutexes[j]);
		sem_destroy(&empties[j]);
		sem_destroy(&fulls[j]);
	}
	return 0;
}

// ----------------------------------------------------------------------------
// PRODUCER:
//		Your producers should be able to constantly produce items up to 1024 
//		in each shared buffer. If all buffers are full and it is the producers 
//		turn to run it will yield, put itself to sleep.  If there are no more 
//		items to produce the producer should print “Producer Thread # is 
//		finished “  and terminate itself.
// ----------------------------------------------------------------------------
void producer(void *num) {
	int item;
	while (num_of_items > 0) {
		item = produce_item();
		
		int i;
		int buffer_num = 0; // will hold value of a not-full shared buffer
		for (i = 0; i < num_of_buffers; i++)
		{
			if (buffer_idx[i] != N) // if shared buffer is not full
			{
				buffer_num = i;
				break;
			}
		}
		
		// grab semaphores of specified buffer
		sem_t *mutex = (sem_t*)&mutexes[buffer_num];
		sem_t *empty = (sem_t*)&empties[buffer_num];
		sem_t *full = (sem_t*)&fulls[buffer_num];

		//int a;
		//sem_getvalue(&fulls[0],&a);
		//printf("Val = %d\n",a);
		//exit(0);
		sem_wait(&empty);
		sem_wait(&mutex);
		insert_item(buffer_num,item);
		
		// decrement number of items
		num_of_items--;
		
		//printf("%s Produced: %d\n",(char*) num,item);
		sem_post(&mutex);
		sem_post(&full);
	}
	printf("Producer Thread %s is finished\n",(char*)num);
}

int produce_item() {
	return rand();
}

void insert_item(int num, int item) {
	bufferList[num][buffer_idx[num]++] = item;
}

// ----------------------------------------------------------------------------
// CONSUMER:
//		Your consumer should be able to read from any of the shared buffers 
//		when it is its turn to run. If all buffers are empty and the 
//		BufferPrinter thread is still running and it is the consumers turn to 
//		run it will print “Consumer Thread # is Yielding “ and will yield, put 
//		itself to sleep.  If there are no more items to consume, the BufferPrinter 
//		is not running the consumer should print that it has finished, print 
//		“Consumer Thread # is finished “ and terminate itself.
// ----------------------------------------------------------------------------
void consumer(void * num) {
	int item;

	while (num_of_items > 0) {
		int i;
		int buffer_num = 0;
		for (i = 0; i < num_of_buffers; i++)
		{
			if (buffer_idx[i] != N)
			{
				buffer_num = i;
				break;
			}
		}

		sem_t *mutex = (sem_t*)&mutexes[buffer_num];
		sem_t *empty = (sem_t*)&empties[buffer_num];
		sem_t *full = (sem_t*)&fulls[buffer_num];
	
		
		sem_wait(&full);
		sem_wait(&mutex);
		//if (buffer_idx[buffer_num] != 0) {
			item = remove_item(buffer_num);
			consume_item(item);
			//printf("%s Consumed %d\n",(char*) num,item);
		/}
		sem_post(&mutex);
		sem_post(&empty);
	}
	printf("Consumer Thread %s is Finished\n",(char*)num);
}

int remove_item(num) {
	return bufferList[num][--buffer_idx[num]];
}

void consume_item(int item) {
	//printf("Consumed: %d\n",item);
}

// ----------------------------------------------------------------------------
// BUFFERPRINTER:
//		The bufferPrinter should print the status of the shared buffers after 
//`		every 1000 items have been created. Hence after the producers have 
//		combined created a 1000 items they should all wait for the BufferPrinter 
//		to print the status of each of shared buffers before creating more items. 
//		After the next 1000 items, total 2000 items, have been created they will 
//		wait until the bufferPrinter prints out the status message again and 
//		repeat this process every 1000 items.  The BufferPrinter should print 
//		something to the effect as
// ----------------------------------------------------------------------------
void bufferprinter()
{
	int last = 0;
	int items;
	int i;
	while(num_of_items > 0) 
	{
		if ((maximum - num_of_items) / 1000 > last)
	       	{
			printf("%d Items Created\n",maximum - num_of_items);
			for (i = 0; i < num_of_buffers; i++)
			{
				printf("Buffer %d has %d items\n",i+1,buffer_idx[i]);
			}			
			last = (maximum - num_of_items) / 1000;
		}
	}

	printf("%d Items Created\n",maximum);
	for (i = 0; i < num_of_buffers; i++)
	{
		printf("Buffer %d has %d items\n",i+1,buffer_idx[i]);
	}			
}
