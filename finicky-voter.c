#include "finicky-voter.h"
/*
 * Liam Hannigan
 *
 * CS441/541: Project 5
 *
 */
//Calls the voters
void *driver_thread(void *arg) {
   voting_count = 0;
   tester = 0;
    change = 0;
    semaphore_wait(&line_start);//end of sign on signal this for one thread
    while(1){
           int ind_tracker = 1;
             int i;
	     tester = 0;
           semaphore_wait(&queue_sem); //queue_sem is for updating queue
             for(i = 0; i < queue_count; i++){
		      if(!tester && !voters[i]->vote_bool){
			      tester = 1; 
			      party=voters[i]->party;
			      voters[i]->vote_bool = 1; //currently voting
                              voting_count++;
			      semaphore_post(&voters[i]->sem);				  
		      }
		      else if(!tester){//do nothing
                      ;}	      
			else if (!voters[i]->vote_bool && (voters[i]->party == party || (voters[i]->party ==  'I' && ind_tracker))) {
                                   voters[i]->vote_bool = 1; //currently voting
                                   voting_count++;
				   semaphore_post(&voters[i]->sem);
                           }
                          else if(voting_count > (1 * num_booths)){
                                   //so wait times dont get too long
                                             break;
                           }
                          else{  //so independent doesnt get special advantage if they line up late
                        	ind_tracker = 0;
                	   }
	     }
            if(voting_count == 0){
                change = 1; 
	     }
             semaphore_post(&queue_sem);		
             if(queue_count  == num_voters && voting_count == 0){
		         break;
	     }
	    if(voting_count == 0){
		    //No one in queue
                    semaphore_wait(&mutex_wait_count); 
	    }
	    else{
		      //Wait for party switch
		       semaphore_wait(&cond);
	    }
   	}
		return NULL;
}

void *voter_thread(void *arg) {
        Voter *voter = (Voter *)arg;
        sign_in(voter);
        wait_vote(voter);
        return NULL;
}

void wait_vote(Voter *voter) {
    semaphore_wait(&queue_sem);
   if(change){
	   //If queue no longer empty after empty check
	   semaphore_post(&mutex_wait_count);
   }
    //If first
    //Add to next spot in queue
    voters[queue_count] = voter;
    queue_count++;
    //First thing
    if(queue_count == 1){
              party = voter->party;
	     only_voter = 0;
             semaphore_post(&line_start);
    } 
    semaphore_post(&queue_sem);
	    
    semaphore_wait(&voter->sem);
     //Critical section
    voter->vote_bool = 1;
    wait_for_booth(voter);

  
     semaphore_wait(&queue_sem);
     voting_count--;
 
    //If ready for a new batch of voters
     if(voting_count == 0){
	semaphore_post(&cond);		  
    }
     semaphore_post(&queue_sem);
     return;
}

void wait_for_booth(Voter *voter) {
    // Waiting in line for a booth
    semaphore_wait(&output);
    print_state(voter, 0);
     semaphore_post(&output);
    int booth_num;
	
    // Acquire the semaphore, which represents the number of available booths
    semaphore_wait(&booth_sem);
    // Find the first available booth
     semaphore_wait(&output);
    for (booth_num = 0; booth_num < num_booths; booth_num++) {
        if (booths[booth_num]->state == 'e') {
            booths[booth_num]->state = voter->party;
          voter->booth = booth_num;
            print_state(voter, 1);
            break;
        }
       else if(booth_num == (num_booths - 1)){
                 booth_num = 0;
 		printf("Error\n");
         }
       }
    semaphore_post(&output);
    // Wait for a random amount of time while voting
    usleep(rand() % 100001);
    // Mark the booth as available and print leaving booth
    semaphore_wait(&output);
    booths[booth_num]->state = 'e';
    print_state(voter, 2);
     semaphore_post(&output);
    // Release the semaphore, indicating that a booth is available
    semaphore_post(&booth_sem);
   return;
 }

//Parse input
void parse(int argc, char*argv[]){
  num_booths = 10;
  num_dems = 5;
  num_reps = 5;
  num_inds = 5;
  num_libs = 0;
  //NEED: check if input is correct
  if (argc > 1) {
      num_booths = atoi(argv[1]);
  }
  if (argc > 2) {
      num_dems = atoi(argv[2]);
  }
  if (argc > 3) {
      num_reps = atoi(argv[3]);
  }
  if (argc > 4) {
      num_inds = atoi(argv[4]);
  }
  if (argc > 5) {
      num_libs = atoi(argv[5]);
  }
  num_voters = num_dems + num_reps + num_inds + num_libs;
  printf("Number of Voting Booths: %d\nNumber of Democrats: %d\nNumber of Republicans: %d\n Number of Independants:\n %d\nNumber of Libertarians: %d\n", num_booths, num_dems, num_reps, num_inds, num_libs);
  srand(time(NULL));
  return;
}


void sign_in(Voter* voter) {
	semaphore_wait(&output);
         print_state(voter, -1);
        semaphore_post(&output);
          barrier_wait();
           sleep(2);
           semaphore_wait(&output);
            if (firstW == 0) {
                   firstW = 1;
                   printf("-----------------------+-----------------------+--------------------------------\n");
            }
            print_state(voter, -2);
           semaphore_post(&output);
           return;
}

void barrier_init() {
    barrier->count = num_voters;
    barrier->arrived = 0;
    queue_count = 0;
    pthread_mutex_init(&barrier->mutex, NULL);
    semaphore_create(&barrier->sem, 0);
    semaphore_create(&booth_sem, num_booths);
    return;
}

void barrier_wait() {
     pthread_mutex_lock(&barrier->mutex);
    barrier->arrived++;
    if (barrier->arrived == barrier->count) {
        int i;
        for (i = 0; i < barrier->count; i++) {
            semaphore_post(&barrier->sem);
        }
        barrier->arrived = 0;
    }
    pthread_mutex_unlock(&barrier->mutex);
    // Block this thread until all threads have arrived
    semaphore_wait(&barrier->sem);
    return;
}

int main(int argc, char *argv[]) {
         semaphore_create(&output, 1);
         semaphore_create(&mutex_wait_count, 0);
	semaphore_create(&cond, 0);
	 semaphore_create(&queue_sem, 1);
         semaphore_create(&line_start, 0);
         firstW = 0;
         only_voter = 1;
	barrier = (Barrier *)malloc(sizeof(Barrier));
	 parse(argc, argv);
	voters = (Voter**) malloc(num_voters * sizeof(Voter));
        printf("-----------------------+-----------------------+--------------------------------\n");
	barrier_init();
	create_voters();
}

void create_voters() {
	booths = malloc(num_booths * sizeof(vBooths*));
	int i;
	 for (i = 0; i < num_booths; i++) {
                  booths[i] = malloc(sizeof(vBooths));
                 booths[i]->state = 'e';
   	}

    // Allocate memory for voter threads
     pthread_t * driver = (pthread_t *)malloc(sizeof(pthread_t));
    dems = (pthread_t *)malloc(num_dems * sizeof(pthread_t));
    reps = (pthread_t *)malloc(num_reps * sizeof(pthread_t));
    inds = (pthread_t *)malloc(num_inds * sizeof(pthread_t));
    libs = (pthread_t *)malloc(num_inds * sizeof(pthread_t));

 pthread_create(&driver[0], NULL, driver_thread, NULL);

  //Create Democratic voter threads
    for (i = 0; i < num_dems; i++) {
      Voter *voter = (Voter *)malloc(sizeof(Voter));
      voter->id = i;
      voter->party = 'D';
      voter->vote_bool = 0;
      voter->booth = -1;
       semaphore_create(&voter->sem, 0);
      pthread_create(&dems[i], NULL, voter_thread, voter);
    }

    //Create Republican voter threads
    for (i = 0; i < num_reps; i++) {
      Voter *voter = (Voter *)malloc(sizeof(Voter));
      voter->id = i;
	voter->vote_bool = 0;
      voter->party = 'R';
      voter->booth = -1;
       semaphore_create(&voter->sem, 0);
      pthread_create(&reps[i], NULL, voter_thread, voter);
    }
    // Create Independent voter threads
    for (i = 0; i < num_inds; i++) {
       Voter *voter = (Voter *)malloc(sizeof(Voter));
       voter->id = i;
       voter->party = 'I';
	voter->vote_bool = 0;
       voter->booth = -1;
       semaphore_create(&voter->sem, 0);
       pthread_create(&inds[i], NULL, voter_thread, voter);
    }
 // Create Libertarian voter threads
    for (i = 0; i < num_libs; i++) {
       Voter *voter = (Voter *)malloc(sizeof(Voter));
       voter->id = i;
       voter->party = 'L';
	voter->vote_bool = 0;
       voter->booth = -1;
       semaphore_create(&voter->sem, 0);
       pthread_create(&libs[i], NULL, voter_thread, voter);
    }
        for (i = 0; i < num_dems; i++) {
        pthread_join(dems[i], NULL);
    }
    for (i = 0; i < num_reps; i++) {
        pthread_join(reps[i], NULL);
    }
    for (i = 0; i < num_inds; i++) {
        pthread_join(inds[i], NULL);
    }
 for (i = 0; i < num_libs; i++) {
        pthread_join(libs[i], NULL);
    }
    pthread_join(driver[0], NULL);
   return;
}

void print_state(Voter* voter, int bool) {
   if(voter->party == 'R'){
       printf("  Republican" );
    }
   else if(voter->party == 'D'){
	printf("    Democrat" );
    }
    else{
	printf(" Independent" );
     }
    printf(" %2d is ", voter->id);
    if(bool == 1){
      printf("   in %d <-| ", voter->booth);
    }
    else{
      printf("        |-> ");
    }
   int i;
    for (i = 0; i < num_booths; i++) {
        printf("[%c]", booths[i]->state == 'e' ? '.' : booths[i]->state);
    }
    printf("  <-|  ");
    if(bool == 0){
      printf("Waiting for a Voting Booth\n");
    }
    else if (bool == 2) {
        printf("Leaving the voting station %d\n", voter->booth);
    } else if(bool == 1){
        printf("Voting! %d.\n", voter->booth);
    }
   else if(bool == -1){
 	printf("Waiting for polling station to open... \n");
    }
    else{
 	printf("Entering the polling station \n");
    }
    return;
}
