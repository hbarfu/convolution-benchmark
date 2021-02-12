//
//  util.h
//  OptimizationTests
//
//  Created by Hannes on 05.02.21.
//  Copyright © 2021 hbarfu. All rights reserved.
//

#ifndef util_h
#define util_h

#include <stdio.h>
#include <Accelerate/Accelerate.h>

typedef struct benchmark {
    const char *name;
    double wallTime;
    double cpuTime;
    double idleTime;
    int num_threads;
    int iterations;
    int fftsize;
} benchmark;

double getWallTime(void);
double getCPUTime(void);
unsigned int getNumCoresWithoutOmp(void);
float* getRandomArray(int n);
DSPSplitComplex * getRandomComplexArray(int n);

#endif /* util_h */
