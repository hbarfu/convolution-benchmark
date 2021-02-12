//
//  defs.h
//  OptimizationTests
//
//  Created by Hannes on 08.02.21.
//  Copyright © 2021 hbarfu. All rights reserved.
//

#ifndef defs_h
#define defs_h

#define LENGTH 1024 //FFT length to compute
#define NUM_FFTS_PER_TEST 1 //number of FFTs to compute. Note: this might be adjusted somewhat to fit number of threads.
#define TEST_ITERATIONS 10 //number of test iterations (more iterations increase test accuracy)
//#define NUM_THREADS 1 //uncomment to explicitly set number of threads. Default number of threads is = number of logical cores

//#define TEST_OMP //uncomment to enable OMP.

#define OPTIMIZATION_TESTS_VERSION "v0.1"

#endif /* defs_h */
