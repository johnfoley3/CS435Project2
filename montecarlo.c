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

 /* Time to get down to business. We need to grab exactly two arguments
 	for the program.
 	@arg numThreads The number of threads to run.
 	@arg numIterations The number of iterations to run the algorithm.

 