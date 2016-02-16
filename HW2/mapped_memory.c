#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>


#define FILE_LENGTH 0x6400

//prototypes
void producer(char*);
void consumer(char*);
int spawn(char*);

int var = 0;

// --- PRODUCER -----------------------------------------------------------------------
//  Will simply increment an integer value in the shared memory
// ------------------------------------------------------------------------------------
void producer(char* file_memory)
{
    int i;
    for (i = 0; i < 10000000000; i++)
    {
      file_memory[0] += 1;
      printf("In spawn, in parent: %d\n", file_memory[0]);
      sprintf((char*) file_memory, "Parent is producing: %d\n", file_memory[0]);

    }
}

// --- CONSUMER -----------------------------------------------------------------------
//  Will simply decrement an integer in the shared memory
// ------------------------------------------------------------------------------------
void consumer(char* file_memory)
{
    int i;
    for (i = 0; i < 10000000000; i++)
    {
      file_memory[0] -= 1;
      printf ("In spawn, in child: %d\n", file_memory[0]);
      sprintf((char*) file_memory, "Child is consuming: %d\n", file_memory[0]);

    }    
}

// --- SPAWN --------------------------------------------------------------------------
//  This method will call the fork() method to create 2 new processes. The parent 
//  process will call the producer() function and the child process will call the
//  the consumer() function. 
// ------------------------------------------------------------------------------------
int spawn(char* file_memory)
{
  pid_t child_pid; //16 bit value ID --> typecast to int(32 bits)
  file_memory[0] = var;
  child_pid = fork();
  int i;

  if(child_pid != 0) //parent
  { 
    producer(file_memory);
    return child_pid;
  }
  else//child
  {
    consumer(file_memory);
    //abort();
  }
  return;
}


int main(int argc, char* const argv[])
{
  int fd;
  void* file_memory;

  //Seed random number gnerator
  srand(time(NULL));

  //Prepate a file large enough to hold unsigned integer
  fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  lseek (fd, FILE_LENGTH+1, SEEK_SET);    //make sure file is large enough 
  write (fd, "", 1);
  lseek(fd, 0, SEEK_SET);    //move write pointer back to beginning


  //CREATE MEMORY MAPPING
  file_memory = mmap(0, FILE_LENGTH, PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd); //closing file access in HD

  //Write a random integer to memory-mapped area  --> using sprintf
  //sprintf((char*) file_memory, "%d\n", spawn(file_memory));
  spawn(file_memory);

  //Release the memory --> memory will automatically be released when 
  munmap (file_memory, FILE_LENGTH);

  return 0;

}
