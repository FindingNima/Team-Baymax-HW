#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define N 100
#define TRUE 1
#define FALSE 0

void producer(void);
void consumer(void);
int produce_item(void);
void insert_item(int);
int remove_item(void);
void consume_item(int);

int count = 0;
int items_array[N];
sem_t mutex;
sem_t empty;
sem_t full;

int produce_item() {
	return rand();
}

void insert_item(int item) {
	items_array[count++] = item;
}

int remove_item() {
	return items_array[--count];
}

void consume_item(int item) {
	printf("Consumed: %d\n",item);
}

void producer() {
	int item;
	while (TRUE) {
		item = produce_item();
		sem_wait(&empty);
		sem_wait(&mutex);
		insert_item(item);
		printf("Produced: %d\n",item);
		sem_post(&mutex);
		sem_post(&full);
	}
}

void consumer() {
	int item;

	while (TRUE) {
		sem_wait(&full);
		sem_wait(&mutex);
		item = remove_item();
		consume_item(item);
		sem_post(&mutex);
		sem_post(&empty);
	}
}

int main() {
	int i[2];
	pthread_t thread_a, thread_b;
	i[0] = 1;
	i[1] = 2;
	sem_init(&mutex,0,1);
	sem_init(&empty,0,N);
	sem_init(&full,0,0);
	pthread_create(&thread_a,NULL,(void*)&producer,&i[0]);
	pthread_create(&thread_b,NULL,(void*)&consumer,&i[1]);
	pthread_join(thread_a,NULL);
	pthread_join(thread_b,NULL);
	sem_destroy(&mutex);
	sem_destroy(&full);
	sem_destroy(&empty);	
	return 0;
}

