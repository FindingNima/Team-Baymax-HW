#include <stdio.h>
#include <stdlib.h>
#define N 100
#define TRUE 1
#define FALSE 0

void producer();
void consumer();
int produce_item();
void insert_item(int);
int remove_item();
void consume_item(int);

int count = 0;
int items_array[N];

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
		if (count == N) return;
		insert_item(item);
		count = count + 1;
	}
}

void consumer() {
	int item;
	while (TRUE) {
		if (count == 0) return;
		count = count - 1;
		item = remove_item();
		consume_item(item);
	}
}

int main() {
	//items_array = (int*) malloc(N);
	while (TRUE) {
		producer();
		consumer();
		printf("Iteration Ended\n");
	}
	return 0;
}
