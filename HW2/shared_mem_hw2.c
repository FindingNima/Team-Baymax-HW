// ----------------------------------------------------------------------------------
// SHARED MEMORY - solving the producer consumer problem
//
// Authors: Emily Le, Daniel, Max Wolotsky
// ----------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

// constants
#define TOTAL 1000
#define SNAME "/mysem2"

// global variables
int total_produced;

// prototypes
void producer(int*);
void consumer(int*);

int binary_semaphore_allocation(key_t, int);
int binary_semaphore_deallocate(int);
int binary_semaphore_initalize(int);
int binary_semaphore_wait(int);
int binary_semaphore_post(int);

// global variables
int var = 0;

// --- PRODUCER -----------------------------------------------------------------------
//  Will simply increment an integer value in the shared memory
// ------------------------------------------------------------------------------------
void producer(int* sh_mem)
{
   // int i;
   // for (i = 0; i < 10000000000; i++)
   // {
   //   sh_mem[0] += 1;
   //   printf("In spawn, in parent: %d\n", sh_mem[0]);
   // }

   // open semaphore 
   sem_t *sem = sem_open(SNAME,0);
   
   while (total_produced < TOTAL)
   {
	   //Lock Semaphore
	   sem_wait(sem);
	   
	   printf("Produced %d\n",++sh_mem[0]);
	   total_produced++;

	   //Unlock Semaphore
   	   sem_post(sem);
   }

}

// --- CONSUMER -----------------------------------------------------------------------
//  Will simply decrement an integer in the shared memory
// ------------------------------------------------------------------------------------
void consumer(int* sh_mem)
{
    // int i;
    // for (i = 0; i < 10000000000; i++)
    // {
    //   sh_mem[0] -= 1;
    //   printf ("In spawn, in child: %d\n", sh_mem[0]);
    // }    

    // open semaphore
    sem_t *sem = sem_open(SNAME,0);

    while (!(sh_mem[0] == 0 && total_produced == TOTAL))
    {
	// Lock Semaphore
	sem_wait(sem);

	if (sh_mem[0] > 0)
		printf("Consumed %d\n",sh_mem[0]--);
	// Unlock Semaphore
    	sem_post(sem);

    }
}

// --- SPAWN --------------------------------------------------------------------------
//  This method will call the fork() method to create 2 new processes. The parent 
//  process will call the producer() function and the child process will call the
//  the consumer() function. 
// ------------------------------------------------------------------------------------
int spawn(int* sh_mem)
{
  pid_t child_pid; //16 bit value ID --> typecast to int(32 bits)
  sh_mem[0] = var;
  child_pid = fork();
  int i;

  if(child_pid != 0) //parent
  { 
    producer(sh_mem);
    return child_pid;
  }
  else//child
  {
    consumer(sh_mem);
    //abort();
  }
  return;
}

// -----SYSTEM SEMAPHORE ----------------------------------------------------------------
//
// --------------------------------------------------------------------------------------
//Semaphore Union 
union semun { 
  int val;
  struct semid_ds *buf;
  unsigned short int *array;
  struct seminfo *__buf;
};

//Obtain binary Semaphore ID, Allocate if necessary
int binary_semaphore_allocation (key_t key, int sem_flags)
{
  return semget(key, 1, sem_flags);
}

//Deallocate binary Semaphore,  "return -1 if fail"
int binary_semaphore_deallocate (int semid)
{
  union semun ignored_argument;
  return semctl(semid,1,IPC_RMID, ignored_argument);
}

//INIT Binary Semaphore with Value 1
int binary_semaphore_initalize (int semid)
{
  union semun argument;
  unsigned short values[1];
  values[0]=1;
  argument.array=values;
  return semctl(semid,0,SETALL,argument);
}

/*******************************************
Wait and Post  --> Down and Up 
*******************************************/

//WAIT
int binary_semaphore_wait(int semid)
{
  struct sembuf operations[1];

  //Use first semaphore --> Only one allocated at this point
  operations[0].sem_num=0;

  //Decrement semaphore by 1
  operations[0].sem_op = -1; 

  //Permit undo  --> If process using sem terminates allows undoes any operation it made on the semaphore
  operations[0].sem_flg = SEM_UNDO;

  return semop(semid, operations, 1);
}


//POST
int binary_semaphore_post(int semid)
{
  struct sembuf operations[1];

//Use first semaphore --> Only one allocated at this point
  operations[0].sem_num=0;

  //Increment semaphore by 1
  operations[0].sem_op = 1; 

  //Permit undo
  operations[0].sem_flg = SEM_UNDO;

  return semop(semid, operations, 1);
}


//---- MAIN -----------------------------------------------------------------------------
//  THis is the main method that will create, allocate and destroy the shared memory
// --------------------------------------------------------------------------------------
int main()
{
  int segment_id;    //ID to Shared Memory Segment
  //char* shared_memory;    //Starting Address of Shared Memory  Recall char = 1 byte
  int* shared_memory;
  struct shmid_ds shmbuffer;
  int segment_size;
  //const int shared_segment_size= 0x3D0900;    //bytes allocate rounded up to integer multip of page size
  const int shared_segment_size = sizeof(int) * (2 + 1000); // Size doesn't really matter, need one point

  // create semaphore
  sem_t *sem = sem_open(SNAME,O_CREAT,0644,1);
  
  //ALLOCATE SHARED MEMORY SEGMENT
  segment_id = shmget(IPC_PRIVATE, shared_segment_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);


  //ATTACH SHARED MEMORY SEGMENT
  //shared_memory = (char*) shmat (segment_id, 0, 0);
  shared_memory = (int*) shmat(segment_id, NULL,0);
  printf( " shared memory attached at address %p\n", shared_memory);

  //DETERMINE SHARED MEMORY SEGMENT SIZE
  shmctl (segment_id, IPC_STAT, &shmbuffer);
  segment_size = shmbuffer.shm_segsz;
  printf( "Shared Memory Segment Size = %d\n", segment_size);

  //spawn
  int child_status = spawn(shared_memory);
  wait(&child_status);


  //DETACH THE SHARED MEMORY SEGMENT  
  shmdt(shared_memory);


  //REATTACH SHARED MEMORY SEGMENT, AT A DIFFERENT ADDRESS!!!   //Specify Page Address in process Adress Space, 0x50000, to attach the shared memory
  shared_memory = (int*)shmat(segment_id,(void*)0x50000,0);
  printf("shared memory reattached at address %p\n", shared_memory);

  
  //PRINT OUT STRING FROM SHARED MEMORY
  printf("In main, [0] = %d\n", shared_memory[0]);
  //printf( "%s\n", shared_memory+sizeOf(int));
  
  //DETACH SHARED MEMORY
  shmdt(shared_memory);

  //DEALLOCATE THE SHARED MEMORY SEGMENT   ****(Don't Forget This Step!!!!)****
  shmctl(segment_id, IPC_RMID, 0);

  return 0;
}
