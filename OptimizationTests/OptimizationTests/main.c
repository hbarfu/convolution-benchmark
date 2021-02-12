//
//  main.c
//  OptimizationTests
//
//  Created by Hannes on 05.02.21.
//  Copyright © 2021 hbarfu. All rights reserved.
//

#include <stdio.h>
#include "fft_vDSP.h"
#include "fft_fftw.h"
#include "util.h"
#include "defs.h"

void notEnoughArguments() {
    printf("⚠️: Not all arguments passed - testing with default values.\n");
    printf("usage:\n");
    printf("OptimizationTests fftSize numConvolutions numIterations (numThreads)\n");
    printf("\tfftSize: the size of the fft to be tested. Must be a power of 2.\n");
    printf("\tnumConvolutions: the number of convolution tasks to execute per test.\n");
    printf("\tnumIterations: the number of times to execute the test (more iterations increase accuracy).\n");
    printf("\t(optional) numThreads: the number of threads to create. If not stated, one thread for each CPU core is created.\n");
}

int main(int argc, const char * argv[]) {
    printf("\n-----------------------\n");
    printf("OptimizationTests %s\n\n", OPTIMIZATION_TESTS_VERSION);
#ifndef TEST_OMP
    printf("🛑: It is discouraged to unset TEST_OMP, because OMP's omp_get_wtime() method accurately measures wall clock time, while Mac Os' built-in clock_gettime(CLOCK_MONOTONIC, ...) seems to NOT count time spend in suspend mode, although it should. This makes measurements somewhat inaccurate!\n\n");
#endif
    
    if(argc < 4) {
        notEnoughArguments();
    }
    int length = argc < 2 ? LENGTH :  (int)strtol(argv[1], NULL, 10);
    int num_convolutions = argc < 3 ? NUM_FFTS_PER_TEST : (int)strtol(argv[2], NULL, 10);
    int iterations = argc < 4 ? TEST_ITERATIONS : (int)strtol(argv[3], NULL, 10);
    int user_num_threads = argc < 5 ? -1 : (int)strtol(argv[4], NULL, 10);
    makeBZ(length);
    
    benchmark b_vdsp = {.wallTime = 0, .cpuTime = 0, .idleTime = 0, .name = NULL, .num_threads = 0, .iterations = 0, .fftsize = 0};
    benchmark b1;
    benchmark b_pthread = {.wallTime = 0, .cpuTime = 0, .idleTime = 0, .name = NULL, .num_threads = 0, .iterations = 0, .fftsize = 0};
    benchmark b2;
    #ifdef TEST_OMP
        benchmark b_omp = {.wallTime = 0, .cpuTime = 0, .idleTime = 0, .name = NULL, .num_threads = 0, .iterations = 0, .fftsize = 0};
        benchmark b3;
    #endif
    
    printf("-----------------------\n");
    for(int i = 0; i < iterations; i++) {
        b1 = fft_vDSP_test(length, num_convolutions);
        b_vdsp.fftsize = b1.fftsize; b_vdsp.iterations = b1.iterations; b_vdsp.name = b1.name; b_vdsp.num_threads = b1.num_threads; b_vdsp.wallTime += b1.wallTime; b_vdsp.cpuTime += b1.cpuTime; b_vdsp.idleTime += b1.idleTime;
    }
    for(int i = 0; i < iterations; i++) {
        b2 = fft_vDSP_test_pthread(length, num_convolutions, user_num_threads);
        b_pthread.fftsize = b2.fftsize; b_pthread.iterations = b2.iterations; b_pthread.name = b2.name; b_pthread.num_threads = b2.num_threads; b_pthread.wallTime += b2.wallTime; b_pthread.cpuTime += b2.cpuTime; b_pthread.idleTime += b2.idleTime;
    }
#ifdef TEST_OMP
    for(int i = 0; i < iterations; i++) {
        b3 = fft_vDSP_test_omp(length, num_convolutions, user_num_threads);
        b_omp.fftsize = b3.fftsize; b_omp.iterations = b3.iterations; b_omp.name = b3.name; b_omp.num_threads = b3.num_threads; b_omp.wallTime += b3.wallTime; b_omp.cpuTime += b3.cpuTime; b_omp.idleTime += b3.idleTime;
    }
#endif
    b_vdsp.wallTime /= iterations;
    b_vdsp.cpuTime /= iterations;
    b_vdsp.idleTime /= iterations;
    b_pthread.wallTime /= iterations;
    b_pthread.cpuTime /= iterations;
    b_pthread.idleTime /= iterations;
#ifdef TEST_OMP
    b_omp.wallTime /= iterations;
    b_omp.cpuTime /= iterations;
    b_omp.idleTime /= iterations;
#endif
    printf("target time: %fms\n\n", (length / 44100.0) * 1000);
    printf("⏱ %s time: \nexecution time: %fms\ncpu time: %fms\nidle time: %fms\n\t@%d executions of %dpt (ctoz, FFT, iFFT, ztoc) spread on %d threads\n\n", b_vdsp.name, b_vdsp.wallTime*1000, b_vdsp.cpuTime*1000, b_vdsp.idleTime*1000, b_vdsp.iterations, b_vdsp.fftsize, b_vdsp.num_threads);
    printf("⏱ %s time: \nexecution time: %fms\ntotal cpu time: %fms\nidle time per thread: %fms\n\t@%d executions of %dpt (ctoz, FFT, iFFT, ztoc) spread on %d threads\n\n", b_pthread.name, b_pthread.wallTime*1000, b_pthread.cpuTime*1000, b_pthread.idleTime*1000, b_pthread.iterations, b_pthread.fftsize, b_pthread.num_threads);
#ifdef TEST_OMP
    printf("⏱ %s time: \nexecution time: %fms\ntotal cpu time: %fms\nidle time per thread: %fms\n\t@%d executions of %dpt (ctoz, FFT, iFFT, ztoc) spread on %d threads\n\n", b_omp.name, b_omp.wallTime*1000, b_omp.cpuTime*1000, b_omp.idleTime*1000, b_omp.iterations, b_omp.fftsize, b_omp.num_threads);
#endif
    return 0;
}
