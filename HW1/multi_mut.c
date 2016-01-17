#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define N 100
#define TRUE 1
#define FALSE 0

sem_t mutex;
int counter;

int produce_item() {
	return rand();
}
void insert_item(int item) {
	items_array[count] = item;
}
int remove_item() {
	return items_array[count];
}
void consume_item(int item) {
	printf("%d\n",item);
}
void producer() {
	int item;
	while (TRUE) {
		item = produce_item();
		while (count == N) {
			pthread_yield();
		}
		sem_wait(&mutex);
			
}
