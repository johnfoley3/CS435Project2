/*
 * Author: John Foley
 * Date: 11.5.14
 * 
 * A program to approximate pi using the Monte Carlo algorithm
 */
 #include <pthread.h>
 #include <stdlib.h>
 #include <stdio.h>

 /* Constants! */
 int throws;
 int hits;

 /* Locks! 
  * pthread_mutex_t * name_lock
  * Note: these are only declarations. Memory allocation occurs in main()
  */
 pthread_mutex_t * rand_lock;
 pthread_mutex_t * throws_lock;

 /* Conditional Variables!
  * pthread_cond_t * cond_var
  * Note: These are only declarations: Memory allocation and 
  * initialization occurs in main()
  */
  pthread_cond_t * can_print;

/*
 * printer()
 *
 * A function/thread to handle printing the aproximations
 */
 int printer() {
 	printf("I'm a printer thread!");
 	return 0;
 }

/*
 * simulation()
 *
 * A function/thread to handle generating points, testing if that point
 * is a hit inside the circle, and test to see if it was a multiple of 
 * million hit
 */
 int simulation() {
 	printf("I'm a simulation thread!");
 	return 0;
 }

 /* Time to get down to business. We need to grab exactly two arguments
 	for the program.
 	@arg numThreads The number of threads to run.
 	@arg numIterations The number of iterations to run the algorithm.
 */
 int main(int argc, char *argv[]) {

 	int done = 0;
 	int numThreads = 0, numIterations = 0;


 	if (argc != 3) {

 		// Too many arguments! 
 		printf("Too many arguments!\n");
 		exit(-1);
 	}

 	// Scan in the correct information
 	// argv[1]: Number of threads to use
 	// argv[2]: Number of iterations to run through the algorithm
 	sscanf(argv[1], "%d", &numThreads);
 	sscanf(argv[2], "%d", &numIterations);

 	// Memory allocation for the locks
 	rand_lock = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
 	pthread_mutex_init (rand_lock, NULL);

 	throws_lock = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
 	pthread_mutex_init (throws_lock, NULL);

 	// Memory allocation for the cond vars
 	can_print = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
 	pthread_cond_init (can_print, NULL);

 	return 0;

 }
