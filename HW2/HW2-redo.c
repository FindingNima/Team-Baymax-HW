// ----------------------------------------------------------------------------
// HW2: Consumer Producer Problem with multiple buffers using semaphores
// Authors: Emily Le, Max Wolotsky, Daniel
// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define TRUE 1
#define FALSE 0
#define DEBUG 0
#define BUFFER_SIZE 1024
#define PRINT_1000 1000

// global variables
int num_of_producers;		// number of producers
int num_of_consumers;		// number of consumers
int num_of_buffers;			// number of buffers
int num_of_items;  			// number of items (will decrement)

int *shared_buffers;

int max_items;
int total_produced = 0;		

sem_t mutex;				// shared buffer lock (critical zone)
sem_t items_to_be_consumed;	
sem_t when_to_print;
sem_t locking_magic;

pthread_t *prod_thread_id;
pthread_t *cons_thread_id;
pthread_t buffer_printer_id;

// prototypes
void producer(void*);
void consumer(void*);
void bufferPrinter();

// ----------------------------------------------------------------------------
// PRODUCER:
//		Your producers should be able to constantly produce items up to 1024 
//		in each shared buffer. If all buffers are full and it is the producers 
//		turn to run it will yield, put itself to sleep.  If there are no more 
//		items to produce the producer should print “Producer Thread # is 
//		finished “  and terminate itself.
// ----------------------------------------------------------------------------
void producer(void* prod_num)
{
	#ifdef DEBUG
	printf("producer is working\n");
	printf("Total produced: %d\n", total_produced);
	#endif

	int i;
	while (total_produced < num_of_items)
	{
		#ifdef DEBUG
		int whenToPrint;
		sem_getvalue(&when_to_print, &whenToPrint);
		//printf("When to print: %d\n", whenToPrint);
		if (whenToPrint == 0) {
			printf("Producer %d is about to sleep\n", (int)prod_num);
		}
		#endif
		
		int mutexVal;
		sem_getvalue(&mutex, &mutexVal);
		//printf("Mutex in producer: %d\n", mutexVal);

		if (sem_trywait(&mutex) == 0) {
			
			for(i = 0; i < num_of_buffers; i++)
			{
				if(shared_buffers[i] < BUFFER_SIZE)
				{
					shared_buffers[i]++;
					total_produced++;
					sem_wait(&when_to_print);

							#ifdef DEBUG
			int whenToPrint;
			sem_getvalue(&when_to_print, &whenToPrint);
			printf("When to print: %d\n", whenToPrint);
			#endif

					sem_post(&items_to_be_consumed);

					#ifdef DEBUG
					//printf("Added to buffer %d\n", i);
					#endif

					break;
				}
			}

			sem_post(&mutex);
		}
	}
	printf("Producer %d is finished\n", (int)prod_num);
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
void consumer(void* cons_num)
{
	// #ifdef DEBUG
	// printf("consumer is working\n");
	// #endif
	
	// int itemsToBeConsumed;

	// while (TRUE)
	// {
	// 	sem_getvalue(&items_to_be_consumed, &itemsToBeConsumed);
	// 	if (itemsToBeConsumed == 0) {
	// 		printf("Consumer thread %d is yielding\n", (int)cons_num);
	// 	}

	// 	int mutexVal;
	// 	sem_getvalue(&mutex, &mutexVal);
	// 	printf("Mutex in consumer: %d\n", mutexVal);

	// 	sem_wait(&items_to_be_consumed);
	// 	if (sem_trywait(&mutex) == 0) {

	// 		#ifdef DEBUG
	// 		int mutexVal;
	// 		sem_getvalue(&mutex, &mutexVal);
	// 		printf("Mutex: %d\n", mutexVal);
	// 		#endif

	// 		int i;
	// 		for (i = num_of_buffers - 1; i >= 0; i--)
	// 		{
	// 			if (shared_buffers[i] > 0)
	// 			{
	// 				shared_buffers[i]--;

	// 				#ifdef DEBUG
	// 				printf("Removed from buffer %d\n", i);
	// 				#endif

	// 				break;
	// 			}
	// 		}

	// 		sem_post(&mutex);

	// 	} else {
	// 		sem_post(&items_to_be_consumed);
	// 	}
		

	// 	if (total_produced == num_of_items && itemsToBeConsumed == 0) 
	// 	{
	// 		printf("Consumer Thread %d is finished\n", (int)cons_num);
	// 		break;
	// 	}
	// }

	//printf("Broke out of while");


}

// ----------------------------------------------------------------------------
// BUFFERPRINTER:
//		The bufferPrinter should print the status of the shared buffers after 
//`		every 1000 items have been created. Hence after the producers have 
//		combined created a 1000 items they should all wait for the BufferPrinter 
//		to print the status of each of shared buffers before creating more items. 
//		After the next 1000 items, total 2000 items, have been created they will 
//		wait until the bufferPrinter prints out the status message again and 
//		repeat this process every 1000 items. 
// ----------------------------------------------------------------------------
void bufferPrinter()
{
	#ifdef DEBUG
	printf("Bufferprinter is starting\n");
	#endif

	while (total_produced < num_of_items)
	{
		if (total_produced != 0 && total_produced % PRINT_1000 == 0)
		{
			printf("%d items created\n", total_produced);

			int i;
			for (i = 0; i < num_of_buffers; i++)
			{
				printf("Buffer %d holds: %d items\n", i, shared_buffers[i]);
			}

			// magic
			sem_wait(&locking_magic);
			for (i = 0; i < PRINT_1000; i++)
			{
				sem_post(&when_to_print);
			}
			sem_post(&locking_magic);

			// #ifdef DEBUG
			// int whenToPrint;
			// sem_getvalue(&when_to_print, &whenToPrint);
			// printf("When to print: %d\n", whenToPrint);
			// #endif
		}

	}

}

// ----------------------------------------------------------------------------
// MAIN:
//		This is the main method that initilizes every fucking thing.
// ----------------------------------------------------------------------------
int main(int argc, char const *argv[])
{
	if (argc < 5 || argc > 5)  
		return 1;

	// intializing the 4 arguments passed in from the shell
	num_of_producers = atoi(argv[1]);
	num_of_consumers = atoi(argv[2]);
	num_of_buffers = atoi(argv[3]);
	num_of_items = atoi(argv[4]);

	//initialize semaphores
	sem_init(&mutex, 0, 1);
	sem_init(&items_to_be_consumed, 0, 0);
	sem_init(&when_to_print, 0, 1000);
	sem_init(&locking_magic, 0, 1);

	// allocating space for threads and shared buffer space for each shared buffer
	shared_buffers = malloc(num_of_buffers * sizeof(int));
	prod_thread_id = malloc(num_of_producers * sizeof(pthread_t));
	cons_thread_id = malloc(num_of_consumers * sizeof(pthread_t));

	// setting shared_buffer values to 0
	int i;
	for(i = 0; i < num_of_buffers; i++)
	{
		shared_buffers[i] = 0;
	}

	// creating producer, consumer, and bufferPrinter threads
	int prod_num[num_of_producers];
	int cons_num[num_of_consumers];

	for (i = 0; i < num_of_producers; i++) 
	{
		prod_num[i] = i;
		pthread_create(&prod_thread_id[i], NULL, (void*) &producer, (void*) &prod_num[i]);
	}

	for (i = 0; i < num_of_consumers; i++) 
	{
		cons_num[i] = i;
		pthread_create(&cons_thread_id[i], NULL, (void*) &consumer, (void*) &cons_num[i]);
	}

	pthread_create(&buffer_printer_id, NULL, (void*) &bufferPrinter, 0);

	// join threads
	for (i = 0; i < num_of_producers; i++)
	{
		pthread_join(prod_thread_id[i], NULL);
	}

	for (i = 0; i < num_of_consumers; i++)
	{
		pthread_join(cons_thread_id[i], NULL);
	}

	pthread_join(buffer_printer_id, NULL);

	for (i = 0; i < num_of_buffers; i++) {
		printf("%d ", shared_buffers[i]);
	}

	// destroy semaphores
	sem_destroy(&mutex);
	sem_destroy(&items_to_be_consumed);
	sem_destroy(&when_to_print);

	// deallocate space
	free(shared_buffers);
	free(prod_thread_id);
	free(cons_thread_id);
}