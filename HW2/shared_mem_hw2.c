/*
 Example showing SHARED MEMORY









*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

int var = 0;
int spawn(char* sh_mem)
{
  pid_t child_pid; //16 bit value ID --> typecast to int(32 bits)
  sh_mem[0] = var;
  child_pid = fork();
  int i;
  if(child_pid != 0){ //parent

    for (i = 0; i < 10000000000; i++)
    {
      sh_mem[0] += 1;
      printf("In spawn, in parent: %d\n", sh_mem[0]);
    }
    return child_pid;
  }
  else//child
  {
    for (i = 0; i < 10000000000; i++)
    {
      sh_mem[0] -= 1;
      printf ("In spawn, in child: %d\n", sh_mem[0]);
    }    
    //fprintf((stderr), "An error occured in execvp\n");
    abort();
  }
  return;
}

// shared memory is being created and where a fork happens to create 2 processes.
int main()
{
  int segment_id;    //ID to Shared Memory Segment
  char* shared_memory;    //Starting Address of Shared Memory  Recall char = 1 byte
  struct shmid_ds shmbuffer;
  int segment_size;
  const int shared_segment_size= 0x6400;    //bytes allocate rounded up to integer multip of page size

  //ALLOCATE SHARED MEMORY SEGMENT
  segment_id = shmget(IPC_PRIVATE, shared_segment_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);


  //ATTACH SHARED MEMORY SEGMENT
  shared_memory = (char*) shmat (segment_id, 0, 0);
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
  shared_memory = (char*)shmat(segment_id,(void*)0x50000,0);
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