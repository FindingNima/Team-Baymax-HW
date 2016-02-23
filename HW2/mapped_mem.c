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
#include <sys/mman.h>
#include <time.h>

// constants
#define TOTAL 1000
#define SNAME "/mysem3"
#define FILENAME "/mapped_mem_folder"
#define FILE_LENGTH 0x3D0900

//const int shared_segment_size= 0x3D0900;    //bytes allocate rounded up to integer multip of page size

// global variables
int total_produced;

// prototypes
void producer(char*);
void consumer(char*);
void storeInt(int,char*);
int loadInt(char*);

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
void producer(char* sh_mem)
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
	   int shared = 0;
	   //Lock Semaphore
	   sem_wait(sem);
	   printf("Produced %d\n",++shared);
	   total_produced++;

	   //Unlock Semaphore
   	   sem_post(sem);
   }

}

// --- CONSUMER -----------------------------------------------------------------------
//  Will simply decrement an integer in the shared memory
// ------------------------------------------------------------------------------------
void consumer(char* sh_mem)
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


	int shared = 0;
	if (shared > 0)
		printf("Consumed %d\n",shared--);


	// Unlock Semaphore
    	sem_post(sem);

    }
}

// --- SPAWN --------------------------------------------------------------------------
//  This method will call the fork() method to create 2 new processes. The parent 
//  process will call the producer() function and the child process will call the
//  the consumer() function. 
// ------------------------------------------------------------------------------------
int spawn(char* sh_mem)
{
  pid_t child_pid; //16 bit value ID --> typecast to int(32 bits)
  printf("1\n");
  sh_mem[0] = (char)0;
  printf("2\n");
  sh_mem[1] = 0;
  printf("3\n");
  sh_mem[2] = 0;
  printf("4\n");
  sh_mem[3] = 0;
  printf("5\n");
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

int loadInt(char* shared) {
	char first = shared[0];
	char second = shared[1];
	char third = shared[2];
	char fourth = shared[3];
	int val = first << 24 + second << 16 + third << 8 + fourth;
	return val;
}

void storeInt(int val, char* shared) {
	shared[0] = val >> 24;
	shared[1] = (val >> 16) & 0x00FF;
	shared[2] = (val >> 8) & 0x0000FF;
	shared[3] = val & 0x000000FF;
}

//---- MAIN -----------------------------------------------------------------------------
//  THis is the main method that will create, allocate and destroy the shared memory
// --------------------------------------------------------------------------------------
int main()
{
  // create semaphore
  sem_t *sem = sem_open(SNAME,O_CREAT,0644,1);
  
  int fd;
  void* file_memory;
  int value = 0;
  int integer;
  //Seed random number gnerator
  srand(time(NULL));


  //Prepate a file large enough to hold unsigned integer
  fd = open(FILENAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  lseek (fd, FILE_LENGTH+1, SEEK_SET);    //make sure file is large enough 
  write (fd, "", 1);
  lseek(fd, 0, SEEK_SET);    //move write pointer back to beginning


  //CREATE MEMORY MAPPING
  //file_memory = mmap(0, FILE_LENGTH, PROT_WRITE, MAP_SHARED, fd, 0);
  file_memory = mmap(0, FILE_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd); //closing file access in HD


  sscanf (file_memory, "%d", &integer); 
  printf ("Value: %d\n", integer);

  //Write 2 to Memory and Read it
  printf("1\n");
  //sprintf((char*)file_memory, "%d\n",2);
  printf("2\n");
  sscanf((char*)file_memory,"%d",&value);
  printf("3\n");
  printf("%d\n",value);




  //Write a random integer to memory-mapped area  --> using sprintf
  //sprintf((char*) file_memory, "%d\n", spawn(file_memory));
  spawn(file_memory);

  //Release the memory --> memory will automatically be released when 
  munmap (file_memory, FILE_LENGTH);

  return 0;
}
