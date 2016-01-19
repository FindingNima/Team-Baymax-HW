#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define N 100
#define TRUE 1
#define FALSE 0

void producer(void);
void consumer(void);

int mutex = 1;
int counter = 0;

void producer(void) {
	int item;
	item = produce_item();
	down(&mutex);
	insert_item(item);
	up(&mutex);
	counter++;
}
void consumer(void) {
	int item;
	down(&mutex);
	item = remove_item();
	up(&mutex);
	consume_item(item);	
}

