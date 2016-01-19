#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
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
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

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
		pthread_mutex_lock(&mutex);
		while(count >= N) {
			pthread_cond_wait(&cond,&mutex);
		}
		insert_item(item);
		printf("Produced: %d\n",item);
		if (count == 1) pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex);
	}
}

void consumer() {
	int item;

	while (TRUE) {
		pthread_mutex_lock(&mutex);
		while (count == 0) {
			pthread_cond_wait(&cond,&mutex);
		}
		item = remove_item();
		consume_item(item);
		if (count == N-1) pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex);
	}
}

int main() {
	int i[2];
	pthread_t thread_a, thread_b;
	i[0] = 1;
	i[1] = 2;
	pthread_create(&thread_a,NULL,(void*)&producer,&i[0]);
	pthread_create(&thread_b,NULL,(void*)&consumer,&i[1]);
	pthread_join(thread_a,NULL);
	pthread_join(thread_b,NULL);
	return 0;
}

