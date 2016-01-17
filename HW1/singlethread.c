#include <stdio.h>
#include <stdlib.h>
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
		if (count == N) return;
		insert_item(item);
		printf("Produced: %d\n",item);
	}
}

void consumer() {
	int item;
	while (TRUE) {
		if (count == 0) return;
		item = remove_item();
		consume_item(item);
	}
}

int main() {
	while (TRUE) {
		producer();
		consumer();
		printf("Iteration Ended\n");
	}
	return 0;
}
