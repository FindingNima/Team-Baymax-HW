#include <stdio.h>
#include <stdlib.h>
#include<pthread.h>

void* print_message(void* ptr);

int main() {
	pthread_t thread1, thread2;
	const char* message1 = "Thread 1";
	const char* message2 = "Thread 2";
	int ret1, ret2;
	
	ret1 = pthread_create(&thread1,NULL,print_message,(void*)message1);
	if (ret1) {
		fprintf(stderr,"Error with pThread create: Code %d\n",ret1);
		exit(EXIT_FAILURE);
	}
	ret2 = pthread_create(&thread2,NULL,print_message,(void*)message2);
	if (ret2) {
		fprintf(stderr,"Error with pThread create: Code %d\n",ret2);
		exit(EXIT_FAILURE);
	}
	printf("Threads return with %d/%d\n",ret1,ret2);
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	
	exit(EXIT_SUCCESS);
	return 0;
}

void *print_message(void* ptr) {
	char* message;
	message = (char*) ptr;
	printf("%s\n",message);
}
