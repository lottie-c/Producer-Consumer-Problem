// Producer

# include "helper.h"
# include <iostream>

using namespace std;



int main (int argc, char *argv[])
{


  time_t start = time(NULL);
  time_t end;
  int time_elapsed;
  
  if(argc != 3){
    cout << "Error: Incorrect number of command line arguments. \n";
    return 0;
  }
  
  int producer_id ;
  int n_jobs;
  int shmid;
  
  producer_id = check_arg(argv[1]);
  n_jobs = check_arg(argv[2]);
  
  if ((producer_id == -1)||(n_jobs == -1)){
    printf("Error: Incorrect command line input. \n");
    return 0;
  }
  
  queue *data; 
  //retrieve and attatch shared memory, cast it to 
  // queue type.
  shmid = shmget(SHM_KEY, SHM_SIZE, 0666 |IPC_CREAT); 
  data = (queue*)shmat(shmid, (void*) 0, 0);
  
  int sem_id = sem_attach(SEM_KEY);
  
  if (sem_id == -1){
    cout << "Error: Semaphore array not attached in producer. \n";
    sem_close(sem_id);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
  }
  
  /*create job, and insert into circular queue when there is a 
   space.*/
  for (int i = n_jobs; i > 0; i--){
    
    jobtype a_job;
    a_job.duration = rand() % 6 + 2;
    sem_wait(sem_id, EMPTY);
    sem_wait(sem_id, MUTEX);
    
    a_job.id = (data->end)+1;
    data->job[data->end] = a_job;
    
    data->end = ((data->end) + 1) % data->size;
   
    end = time(NULL);
    time_elapsed=int( end - start);

    cout << "Producer(" << producer_id << ") time " << time_elapsed
	 << ": Job id " << a_job.id << " duration " 
	 << a_job.duration << "\n";
 
  
    sem_signal(sem_id, MUTEX);
    sem_signal(sem_id, FULL);

    sleep(rand()%3 + 2);    
  }
  
  
  end = time(NULL);
  time_elapsed=int( end - start);
  cout << "Producer(" << producer_id << ") time " << time_elapsed
       << ": No more jobs to generate \n";
  /*sleep for 500 seconds, could be adjusted depending on number of
   jobs, producers and consumers*/
  sleep(500);

  shmdt((void*) data);
    
  return 0;
}
