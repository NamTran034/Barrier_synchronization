#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int NUM_THREADS, NUM_BARRIERS;

static void centralized_barrier(int *count, int *sense, int *local_sense) {
	int i;
	int n = 0;

	// busy-waiting delay
	for (i = 0; i < 50000; i++){
		n += 1;
	}; 	
	
	// reverse the local sense of the calling thread 
	*local_sense = ! *local_sense; 	 
	

	// create a critical section (block of code that only one 
	// thread can execute at a time) to safely decrement count
	// to make sure that this operation is atomic, meaning that 
	// it happens as a single, uninterruptible operation
	#pragma omp critical 
	{	
		*count = (*count) - 1;
	}
		

	if (*count == 0) {				
        *count = NUM_THREADS;	
        *sense = *local_sense;  // reverse the sense flag
	} else {
       	while (*sense != *local_sense) {}
	}
}

int main(int argc, char **argv) {

	// checking syntax
  	if(argc == 3) {
		NUM_THREADS = atoi(argv[1]);		// ASCII to integer 
		NUM_BARRIERS = atoi(argv[2]);		// ASCII to integer
  	} else {  
		printf("Syntax: ./centralized num_threads num_barriers\n");
		exit(-1);
	}

	omp_set_num_threads(NUM_THREADS);
	int sense = 1, count = NUM_THREADS;
	double time1, time2;

	#pragma omp parallel shared(sense, count) 
	{
    	int num_threads = omp_get_num_threads(); 
    	int thread_num = omp_get_thread_num();		
    	int local_sense = 1;						
    	long i, j;

    	for (j = 0; j < NUM_BARRIERS; j++) {
    		for (i = 0; i < 5000; i++);		// busy waiting delay
    		printf("Hello World from thread %d of %d.\n", thread_num, num_threads);
    		time1 = omp_get_wtime()*1000000;		// start time before entering the barrier
    		centralized_barrier(&count, &sense, &local_sense);	//centralized barrier
    		time2 = omp_get_wtime()*1000000;		// end time after exiting the barrier
    		//Centralized barrier reached by all threads. Continue.
    		for(i=0; i<5000; i++);			// busy waiting delay                   
    		printf("Hello World from thread %d of %d after barrier\n", thread_num, num_threads);
    	}

    	printf("Time spent in barrier by thread %d is %f\n", thread_num, time2 - time1);
	}
	
	//printf("The number of processors on this system is %d", omp_get_numprocs());
	return 0;
}




