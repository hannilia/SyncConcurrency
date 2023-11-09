/*
 * Liam Hannigan
 *
 * CS441/541: Project 5
 *
 */
#ifndef ELECTION_H
#define ELECTION_H
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "semaphore_support.h"
#include <pthread.h>

/******************************
 * Defines
 ******************************/

/******************************
 * Structures
 ******************************/
typedef struct Voter {
    int id;
    char party;
    int vote_bool;
    int booth;
   semaphore_t sem;
} Voter;

typedef struct vBooths {
    char state; //'e' for empty
} vBooths;

typedef struct Barrier {
    int count;             // Number of threads to wait for
    int arrived;           // Number of threads that have arrived at the barrier
    pthread_mutex_t mutex; 
    semaphore_t sem;
} Barrier;

/******************************
 * Global Variables
 ******************************/

void barrier_init();
semaphore_t booth_sem;
semaphore_t output;
semaphore_t empty;
void barrier_wait();
vBooths **booths;
semaphore_t **semaphores;
pthread_t *dems;
pthread_t *reps;
pthread_t *libs;
pthread_t *inds;
Barrier * barrier;
int num_booths;
int num_voters;
int num_dems;
int num_libs;
int num_reps;
int num_inds;
int firstW;
int queue_count;
int voting_count;
char party;
Voter ** voters;
semaphore_t line_start;
semaphore_t queue_sem;
semaphore_t cond;
semaphore_t mutex_wait_count;
int only_voter;
int tester;
int change;

/******************************
 * Function declarations
 ******************************/

void wait_vote(Voter *voter);
void print_state(Voter *voter, int bool);
void *voter_thread(void *arg);
void *driver_thread(void *arg);
void parse(int argc, char *argv[]);
void *voter(void *arg);
void sign_in(Voter *voter);
void vote(Voter *voter);
void wait_for_booth(Voter *voter);
void create_voters();
#endif

