 /*
 * assign2.c
 *
 * Name:Saige Liu
 * Student Number:V00812068
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include "train.h"

/*
 * If you uncomment the following line, some debugging
 * output will be produced.
 *
 * Be sure to comment this line out again before you submit 
 */

/* #define DEBUG	1 */

void ArriveBridge (TrainInfo *train);
void CrossBridge (TrainInfo *train);
void LeaveBridge (TrainInfo *train);

//two mutex
pthread_mutex_t lock_one = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_two = PTHREAD_MUTEX_INITIALIZER;
//one convar
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
TrainInfo* train_on_bridge;//the id of the train currently in the bridge
int bridge_avail = 1;//if the bridge available
int num_waiting_East = 0;
int num_waiting_West = 0;//number of trains that are waiting outside bridge, going to west

int East_go=0;
int triger=0;
TrainInfo* waiting_trains_East[80];
TrainInfo* waiting_trains_West[80];//waiting train go to west, array


/*
 * This function is started for each thread created by the
 * main thread.  Each thread is given a TrainInfo structure
 * that specifies information about the train the individual 
 * thread is supposed to simulate.
 */
void * Train ( void *arguments )
{
	TrainInfo	*train = (TrainInfo *)arguments;

	/* Sleep to simulate different arrival times */
	usleep (train->length*SLEEP_MULTIPLE);

	ArriveBridge (train);

	CrossBridge  (train);

	LeaveBridge  (train); 

	/* I decided that the paramter structure would be malloc'd 
	 * in the main thread, but the individual threads are responsible
	 * for freeing the memory.
	 *
	 * This way I didn't have to keep an array of parameter pointers
	 * in the main thread.
	 */
	free (train);
	return NULL;
}

/*
 * You will need to add code to this function to ensure that
 * the trains cross the bridge in the correct order.
 */
void ArriveBridge ( TrainInfo *train )
{
	printf ("Train %2d arrives going %s\n", train->trainId, 
			(train->direction == DIRECTION_WEST ? "West" : "East"));
	//lock the whole function
	pthread_mutex_lock(&lock_two);
	
	if(train->direction == DIRECTION_WEST){
		//request to enter the bridge, on success, return;	
	if(bridge_avail == 1 && num_waiting_West == 0 && num_waiting_East==0){
		//set current train
		
		train_on_bridge = train;
		//set the bridge unavailable
		bridge_avail--;
		pthread_mutex_unlock(&lock_two);
		return;
	}
		//otherwise, add the train to the waiting list
		pthread_mutex_lock(&lock_one);
	
		waiting_trains_West[num_waiting_West] = train;
		if(num_waiting_West==0)	
			triger=1;
		num_waiting_West++;
		if(triger==1&&train_on_bridge->direction==DIRECTION_EAST){
			East_go=1;
			triger=0;
		}	
		pthread_mutex_unlock(&lock_one);

	
		
		while((train->trainId != waiting_trains_West[0]->trainId) || (bridge_avail != 1)||(num_waiting_East>0&&East_go<2)){
			
			pthread_cond_wait(&cond,&lock_two);
		
		}
		pthread_mutex_lock(&lock_one);
		East_go=0;

		train_on_bridge = train;
	
		int i;
		for (i = 0; i < num_waiting_West; i++)
		
			waiting_trains_West[i] = waiting_trains_West[i+1];
		num_waiting_West--;
		bridge_avail--;//the bridge becomes unavailable
		pthread_mutex_unlock(&lock_one);
	
		
	}else{
			//request to enter the bridge, on success, return;	
	if(bridge_avail == 1 && num_waiting_West == 0 && num_waiting_East==0){
		//set current train
		
		train_on_bridge = train;
		//set the bridge unavailable
		bridge_avail--;
		pthread_mutex_unlock(&lock_two);
		return;
	}
	
		//otherwise, add the train to the waiting list
		pthread_mutex_lock(&lock_one);
	
		waiting_trains_East[num_waiting_East] = train;
	
	
		num_waiting_East++;	
		pthread_mutex_unlock(&lock_one);

	
		//TrainInfo *First=waiting_trains[0];
		//wait, if (1) the train is not at the head of the waiting list or (2) the bridge is unavailable, wait
		while((train->trainId != waiting_trains_East[0]->trainId) || (bridge_avail != 1||(num_waiting_West>0&&East_go>1))){
			
			pthread_cond_wait(&cond,&lock_two);
		
		}
	
		//get out of the waiting list
	
		pthread_mutex_lock(&lock_one);
		if(num_waiting_West>0){
			East_go++;
		}else if(num_waiting_West==0){
			East_go=0;		
		}
		
		train_on_bridge = train;
		int i;
		for (i = 0; i < num_waiting_East; i++)
		
			waiting_trains_East[i] = waiting_trains_East[i+1];
		num_waiting_East--;
		bridge_avail--;//the bridge becomes unavailable
		pthread_mutex_unlock(&lock_one);
	
		
	}
	//release 
	pthread_mutex_unlock(&lock_two);
}

/*
 * Simulate crossing the bridge.  You shouldn't have to change this
 * function.
 */
void CrossBridge ( TrainInfo *train )
{
	printf ("Train %2d is ON the bridge (%s)\n", train->trainId,
			(train->direction == DIRECTION_WEST ? "West" : "East"));
	fflush(stdout);
	
	/* 
	 * This sleep statement simulates the time it takes to 
	 * cross the bridge.  Longer trains take more time.
	 */
	usleep (train->length*SLEEP_MULTIPLE);

	printf ("Train %2d is OFF the bridge(%s)\n", train->trainId, 
			(train->direction == DIRECTION_WEST ? "West" : "East"));

	fflush(stdout);
	
}

/*
 * Add code here to make the bridge available to waiting
 * trains...
 */
void LeaveBridge ( TrainInfo *train )
{	
	pthread_mutex_lock(&lock_one);
	
	
	bridge_avail++;
	
	pthread_mutex_unlock(&lock_one);
	//wake up all the waiting threads
	pthread_cond_broadcast(&cond);
}

int main ( int argc, char *argv[] )
{
	int		trainCount = 0;
	char 		*filename = NULL;
	pthread_t	*tids;
	int		i;

		
	/* Parse the arguments */
	if ( argc < 2 )
	{
		printf ("Usage: part1 n {filename}\n\t\tn is number of trains\n");
		printf ("\t\tfilename is input file to use (optional)\n");
		exit(0);
	}
	
	if ( argc >= 2 )
	{
		trainCount = atoi(argv[1]);
	}
	if ( argc == 3 )
	{
		filename = argv[2];
	}	
	
	initTrain(filename);
	
	/*
	 * Since the number of trains to simulate is specified on the command
	 * line, we need to malloc space to store the thread ids of each train
	 * thread.
	 */
	tids = (pthread_t *) malloc(sizeof(pthread_t)*trainCount);
	
	/*
	 * Create all the train threads pass them the information about
	 * length and direction as a TrainInfo structure
	 */
	for (i=0;i<trainCount;i++)
	{
		TrainInfo *info = createTrain();
		
		printf ("Train %2d headed %s length is %d\n", info->trainId,
			(info->direction == DIRECTION_WEST ? "West" : "East"),
			info->length );

		if ( pthread_create (&tids[i],0, Train, (void *)info) != 0 )
		{
			printf ("Failed creation of Train.\n");
			exit(0);
		}
	}

	/*
	 * This code waits for all train threads to terminate
	 */
	for (i=0;i<trainCount;i++)
	{
		pthread_join (tids[i], NULL);
	}
	
	free(tids);
	return 0;
}

