// ==== Team Members =======================================================
//	* Daniel
//	* Emily Le
//	* Max Wolotsky
// ========================================================================

#include <stdio.h>
#include <stdlib.h>
#define N 100
#define TRUE 1
#define FALSE 0

// prototypes
void producer(void);
void consumer(void);
int produce_item(void);
void insert_item(int);
int remove_item(void);
void consume_item(int);

int count = 0;
int items_array[N];

// produce a random integer that will be added to an array
int produce_item() {
	return rand();	
}


// will put generated random integer into array
void insert_item(int item) {
	items_array[count++] = item;
}

// will remove integer at iteration and will return the value
int remove_item() {
	return items_array[--count];
}

void consume_item(int item) {
	printf("Consumed: %d\n",item);
}


// ==== Producer  ===========================================================
//	If the count reaches N the function will stop running to let the
//	other function (consumer) run. Otherwise the count will continue to
//	increment.
// ==========================================================================
void producer() {
	int item;
	while (TRUE) {
		item = produce_item();
		if (count == N) return;
		insert_item(item);
		printf("Produced: %d\n",item);
	}
}

// === Consumer ============================================================
//	If the Count reaches 0, the function will stop running and allow the
//	other function (Producer) to run. Otherwise the counter will continue
//	to decrement.
// =========================================================================
void consumer() {
	int item;
	while (TRUE) {
		if (count == 0) return;
		item = remove_item();
		consume_item(item);
	}
}

// === main =================================================================
//	The $PC will ooint to this functional initially to start the program.
//	It continuously calls the Producer and Consumer functions forever.	
// ==========================================================================
int main() {
	while (TRUE) {
		producer();
		consumer();
		printf("Iteration Ended\n");
	}
	return 0;
}
