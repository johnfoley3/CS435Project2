/*
 * Author: John Foley
 * Date: 11.5.14
 * 
 * A program to approximate pi using the Monte Carlo algorithm
 * Note: This program requires the -lthread and -lm linker arguments
 *		in the compilation command
 */
 #include <pthread.h>
 #include <stdlib.h>
 #include <stdio.h>
 #include <time.h>
 #include <math.h>

 #define _THRESHOLD 1000000

 /* Constants! */
 int throws = 0;
 int hits = 0;
 int done = 0;
 int numIterations = 0;
 int ok_to_print = 0;

 /* Locks! 
  * pthread_mutex_t * name_lock
  * Note: these are only declarations. Memory allocation occurs in main()
  */
 pthread_mutex_t * rand_lock;
 pthread_mutex_t * throws_lock;
 pthread_mutex_t * print_lock;

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
 void *printer() {

 	float approx_pi = 0.0;

 	while (!done) {

 		pthread_mutex_lock(print_lock);

 		while (! ok_to_print) {

 			/* wait until a simulation thread broadcasts that it's okay to print */
 			pthread_cond_wait (can_print, print_lock);
 		}

 		ok_to_print = 0;

 		/* lock the throws so that we accurately calculate the approximation */
 		pthread_mutex_lock(throws_lock);

 		/* calculate the approximation */
 		approx_pi = (float)hits / (float)throws;

 		pthread_mutex_unlock(throws_lock);

 		/* print the approximation */
 		printf("This is the current approximation of pi: %f, at throw number: %d \n", 4.0 * approx_pi, throws);

 		pthread_mutex_unlock(print_lock);
 	}

 	return 0;
 }

/*
 * simulation()
 *
 * A function/thread to handle generating points, testing if that point
 * is a hit inside the circle, and test to see if it was a multiple of 
 * million hit
 */
 void *simulation() {


 	/* The data points (x,y) */
 	double x = 0;
 	double y = 0;
 	int isHit = 0;

 	while (!done) {
 	
	 	/* rand() is not thread safe */
	 	pthread_mutex_lock(rand_lock);

	 	/* Divide by the max random number so that we get a number in range
	 		[0, 1.0] */
	 	x = (double)rand() / (double)RAND_MAX;
	 	y = (double)rand() / (double)RAND_MAX;

	 	pthread_mutex_unlock(rand_lock);

	 	/* Use this test to see if we hit inside the circle */
	 	if (sqrt(x*x + y*y) <= 1) {

			isHit = 1; 		
	 	} else {

	 		isHit = 0;
	 	}

	 	/* this is a hit */
		pthread_mutex_lock(throws_lock);

	 	throws++;

	 	if (isHit) { hits++; }

	 	/* Should we print yet? 
	 		current threshold is set to every million */
	 	if(throws % _THRESHOLD == 0) {

	 		ok_to_print = 1;
	 		pthread_cond_broadcast(can_print);
	 	}

	 	/* Are we there yet? */
	 	if(throws == numIterations) {

	 		done = 1;
	 	}

	 	pthread_mutex_unlock(throws_lock);
 	}
 	
 	return 0;
 }

 /* Time to get down to business. We need to grab exactly two arguments
 	for the program.
 	@arg numThreads The number of threads to run.
 	@arg numIterations The number of iterations to run the algorithm.
 */
 int main(int argc, char *argv[]) {

 	/* numThreads doesn't need to be global;
 		it's only used in the for loop for the creation fo threads */
 	int numThreads = 0;
 	int i = 0;

 	/* Error checking; not essential, but could be useful */
 	if (argc > 3) {

 		/* Too many arguments!  */
 		printf("Too many arguments!\n");
 		exit(-1);
 	} else if (argc < 3) {

 		/* Too few arguments!  */
 		printf("Too few arguments!\n");
 		exit(-1);
 	}

 	/* Scan in the correct information
 		argv[1]: Number of threads to use
 		argv[2]: Number of iterations to run through the algorithm */
 	sscanf(argv[1], "%d", &numThreads);
 	sscanf(argv[2], "%d", &numIterations);

 	/* Make sure we have one or more threads */
 	if (numThreads < 1) {

 		printf("Not a valid number of threads (must have 1 or more)");
 		exit (-1);
 	}

 	 /* Create our random number generator
	 	Note: the seed is from the time library, rather than some
	 	arbitrary number that I define.  */
	 srand(time(NULL));

 	/* Memory allocation for the locks */
 	rand_lock = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
 	pthread_mutex_init (rand_lock, NULL);

 	throws_lock = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
 	pthread_mutex_init (throws_lock, NULL);

 	print_lock = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
 	pthread_mutex_init (print_lock, NULL);

 	/* Memory allocation for the cond vars */
 	can_print = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
 	pthread_cond_init (can_print, NULL);

 	/* Create the printer thread */
 	pthread_t * printer_thread = (pthread_t *) malloc (sizeof (pthread_t));

 	if (pthread_create (printer_thread,
 							NULL,
 							printer,
 							NULL)) {
 			fprintf(stderr, "Error creating printer thread.\n");
 			exit(-1);
 		}

 	/* Create all of the simluation threads */
 	pthread_t ** simulation_threads = (pthread_t **) malloc ((sizeof (pthread_t)) * numThreads);

 	/* Step through and initialize all the threads */
 	for (i = 0; i < numThreads; i++) {

 		simulation_threads[i] = (pthread_t *) malloc (sizeof (pthread_t));

 		/* start the threads
 			args[1]: the pthread pointer
 			args[2]: NULL
 			args[3]: the function the thread will use
 			args[4]: void pointer to argument (the random number generator) */
 		if (pthread_create (simulation_threads[i],
 							NULL,
 							simulation,
 							NULL)) {
 			fprintf(stderr, "Error creating simulation thread %d\n", i);
 			exit(-1);
 		}
 	}

 	/* Clean up */
 	for (i = 0; i < numThreads; i++) {

 		if (pthread_join (*simulation_threads[i], NULL)) {
 			fprintf (stderr, "Error joining simulation thread %d\n", i);
 			exit (-1);
 		}
 	}

 	exit(0);

 }
