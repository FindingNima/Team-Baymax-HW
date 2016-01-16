#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define N 100
#define TRUE 1
#define FALSE 0

typedef int semaphore;
semaphore mutex = 1;
semaphore empty = N;
semaphore full = 0;

void producer(void);
void consumer(void);

void producer(void) {
	int item;
	
	while (TRUE) {
		item = produce_item();
		down(&empty);
		down(&mutex);
		insert_item(item);
		up(&mutex);
		up(&full);
	}
}
void consumer(void) {
	int item;

	while (TRUE) {
		down(&full);
		down(&mutex);
		item = remove_item();
		up(&mutex);
		up(&empty);
		consume_item(item);
	}
}

