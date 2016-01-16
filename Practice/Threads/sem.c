#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

void handler(void *ptr);

sem_t mutex;
int counter;

int main() {
	counter = 0;
	int i[2];
	pthread_t thread_a,thread_b;
	i[0] = 0;
	i[1] = 1;
	sem_init(&mutex,0,1);
	pthread_create(&thread_a,NULL,(void*)&handler,&i[0]);
	pthread_create(&thread_b,NULL,(void*)&handler,&i[1]);
	pthread_join(thread_a,NULL);
	pthread_join(thread_b,NULL);
	sem_destroy(&mutex);
	return 0;
}
void handler(void* ptr) {
	int x;
	x = *((int*)ptr);
	printf("Thread %d: Waiting to Enter Critical Region\n",x);
	sem_wait(&mutex);
	printf("Thread %d: Now in Critical Region. Counter: %d\n",x,counter);
	counter = counter + 1;
	printf("Thread %d: New Counter: %d\n",x,counter);
	printf("Thread %d: Exiting Critical Region\n",x);
	sem_post(&mutex);
	pthread_exit(0);
}	

