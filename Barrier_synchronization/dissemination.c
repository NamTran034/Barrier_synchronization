#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <stdint.h>
#define MAXTHREADS 10000

int NUM_THREADS, NUM_BARRIERS;

// Structure is defined to hold the flags for each thread and its partner threads.
typedef struct flags {
	int myflags[2][MAXTHREADS];				
	int *partnerflags[2][MAXTHREADS];	
} flags;

void dissemination_barrier( flags *localflags, int *sense, int *parity, int *proc) {
	/*
		localflags: Local flags for the calling thread.
		sense: Current synchronization sense.
		parity: Current parity bit.
		proc: Number of processors (log2 of the number of threads)
	*/

	int p = *parity;	// copy the value of 'parity' to 'p'
	int i;
	for(i = 0; i < *proc; i++) {
		#pragma omp critical 
		{
			*localflags->partnerflags[p][i] = *sense;
		}
		while(localflags->myflags[p][i] != *sense){}
	}

	if(*parity == 1)
		*sense = !*sense;
	*parity = 1 - *parity;
}


int main(int argc, char **argv) {

	// check syntax
	if(argc==3) {
		NUM_THREADS = atoi(argv[1]);		// ASCII to integer
		NUM_BARRIERS = atoi(argv[2]);		// ASCII to integer
	} else {
        printf("Syntax: ./dissemination num_threads num_barriers\n");
        exit(-1);
    }

	omp_set_num_threads(NUM_THREADS);
	double time1, time2;		
	flags allnodes[NUM_THREADS]; 				// array of flags, each is a flag for a thread
	int proc = ceil(log(NUM_THREADS)/log(2));	// number of rounds

	#pragma omp parallel shared(allnodes, proc) 
	{
		int thread_num = omp_get_thread_num();
		int numthreads = omp_get_num_threads();

		int i, j, k, l, m;
		int parity = 0; 	//processor private
		int sense = 1; 		//processor private
		flags *localflags = &allnodes[thread_num]; //processor private
		int temp, y;

		#pragma omp critical
		// initilize local flags for each thread to 0
		for (l = 0; l < NUM_THREADS; l++)
			for (m = 0; m < 2; m++)
				for (k = 0; k < proc; k++)
					allnodes[l].myflags[m][k] = 0;

		for (i = 0; i < NUM_BARRIERS; i++) {
			printf("Hello world from thread %d of %d\n", thread_num, numthreads);

			// initialize partner flags
			#pragma omp critical
			for (j = 0; j < NUM_THREADS; j++) {
				for (k = 0; k < proc; k++) {
					temp = ceil(pow(2,k));
					if (j == (thread_num + temp) % NUM_THREADS) {
						allnodes[thread_num].partnerflags[0][k] =  &allnodes[j].myflags[0][k];
						allnodes[thread_num].partnerflags[1][k] =  &allnodes[j].myflags[1][k];
					}
				}
			}

			time1 = omp_get_wtime()*1000000;
			dissemination_barrier(localflags, &sense, &parity, &proc);
			time2 = omp_get_wtime()*1000000;
			printf("Hello world from thread %d of %d after barrier\n", thread_num, numthreads);
		}
		printf("Time spent in barrier by thread %d is %f\n", thread_num, time2-time1);
	}
}
