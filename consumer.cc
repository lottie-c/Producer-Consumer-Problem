// Consumer

# include "helper.h"
# include <iostream>

using namespace std;

int main (int argc, char *argv[])
{
  
  time_t start = time(NULL);
  time_t end;
  int time_elapsed;

  if(argc != 2){
    cout <<  "Error: Incorrect number of command line arguments. " 
	 <<argc <<" \n";
    return 0;
  }

  int consumer_id;
  consumer_id = check_arg(argv[1]);

  if (consumer_id == -1){
    cout << "Error: Incorrect command line input. \n";
    return 0;
   
  }

  queue *data; 
  int shmid;
  
  //retrieve and attach shared memory, casting it to 
  //queue type.
  shmid = shmget(SHM_KEY, SHM_SIZE, 0666 |IPC_CREAT); 
  data = (queue*)shmat(shmid, (void*) 0, 0);
  
  //attach semaphores
  int sem_id = sem_attach(SEM_KEY);
  if (sem_id == -1){
    cout << "Error: Semaphore array not attached in consumer. \n";
    sem_close(sem_id);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
  }
  /*track number of consumers so as not to prematurely delete the
   shared resources*/
  data->n_consumers++;
 
  /*while there is a ten second wait or less, wait for something 
  to be put in the queue. If something is put in queue consume it */
  while(sem_timewait(sem_id, FULL, 10) != -1){
      
      sem_wait(sem_id, MUTEX);

      jobtype current_job;
      current_job = data->job[data->front];
      data->front = ((data->front) + 1) % data->size;
     
      end = time(NULL);
      time_elapsed=int( end - start);
      
      cout << "Consumer(" << consumer_id << ") time " << time_elapsed
	   << ": Job id " << current_job.id
	   << " executing sleep duration "
	   << current_job.duration <<"\n";
 
   
      sem_signal(sem_id, MUTEX);
      sem_signal(sem_id, EMPTY);
   
    
      sleep(current_job.duration);
    

      end = time(NULL);
      time_elapsed=int( end - start);
      cout << "Consumer(" << consumer_id << ") time " << time_elapsed
	   << ": Job id " << current_job.id << " completed.\n";

    }

  end = time(NULL);
  time_elapsed=int( end - start);

  cout <<"Consumer("<< consumer_id<< ") time " << time_elapsed
       << ": No Jobs left. \n";
  

  sleep (10);

  sem_wait(sem_id, MUTEX);
  data->n_consumers--;
  
  /*if this is the last consumer close the semaphores and 
   dettatch and remove shared memory*/
  if (data->n_consumers  == 0){
    if(sem_close(sem_id) == -1){
      cout << "Error: semaphores not closed "<< endl;
      return 0;
    }
    shmdt((void*)data);
    shmctl(shmid, IPC_RMID, NULL);
  }
  sem_signal(sem_id, MUTEX);
  return 0;
}
