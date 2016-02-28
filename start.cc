/******************************************************************
 * Program for setting up semaphores/shared memory before running
 * the Producer and Consumer 
 ******************************************************************/
//get shared memory please
# include "helper.h"
#include <iostream>

using namespace std;
int main (int argc, char **argv)
{	
  int size = check_arg(argv[1]);
  if (size == -1){
    cout << "Error: Incorrect command line input.";
    return 0;
  }


  int sem_array_id;
  int sem_mutex;
  int sem_full;
  int sem_empty;
  
  int shmid;
  queue *data;


  if((SHM_KEY == -1)||(SEM_KEY == -1)){
    cout << "Error creating semaphore key or shared memory key." ;
    return 0;
  }
  //create shared memory and type cast it to queue
  shmid = shmget(SHM_KEY, SHM_SIZE, 0666 |IPC_CREAT); 
  data = (queue*)shmat(shmid, (void*) 0, 0); 
  *data =(queue){.size = size,.front = 0,.end = 0, .n_consumers = 0};      
  //make a set of 3 semaphores	
  sem_array_id = sem_create(SEM_KEY, 3);
  
  
  if (sem_array_id == -1){
    cout << "Error while creating semaphore array. \n";
    return 0;
  }
  //initialise the semaphores
  sem_mutex = sem_init(sem_array_id, MUTEX, 1);
  sem_empty = sem_init(sem_array_id, EMPTY, size);
  sem_full = sem_init(sem_array_id, FULL, 0);
  
  if (sem_mutex == -1){
    cout << "Error while initialising mutex semaphore. \n";
    sem_close(sem_array_id);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
  }
  if (sem_empty == -1){
    cout << "Error while initialising empty semaphore. \n";
    sem_close(sem_array_id);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
  }
  if (sem_full == -1){
    cout << "Error while initialising full semaphore. \n";
    sem_close(sem_array_id);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
  }

 

  return 0;
}
