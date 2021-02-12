//
//  fft_vDSP.c
//  OptimizationTests
//
//  Created by Hannes on 05.02.21.
//  Copyright © 2021 hbarfu. All rights reserved.
//

#include "fft_vDSP.h"
#include <stdlib.h>
#include "defs.h"
#include <sys/param.h>
#ifdef TEST_OMP
#include "omp.h"
#endif
#include <pthread.h>

//#define NUM_CORES 4

/*Since we assume an already prepared impulse reponse in frequency domain for our convolutions, we use a global array here. This should be thread-safe since the array is never written to from the worker threads.*/
DSPSplitComplex *bz_global;
DSPSplitComplex *makeBZ(int length) {
    bz_global = getRandomComplexArray(length);
    return bz_global;
}




/* ----------------------- SINGLE -THREADED IMPLEMENTATION ----------------------- */

benchmark fft_vDSP_test(int length, int iter) {
    vDSP_Length fftlog2 = (vDSP_Length)log2(length);
    FFTSetup fftSetup = vDSP_create_fftsetup(fftlog2, FFT_RADIX2);
    float* a = getRandomArray(length);
    DSPSplitComplex* az = malloc(sizeof(DSPSplitComplex));
    az->realp = malloc(length/2 * sizeof(float));
    az->imagp = malloc(length/2 * sizeof(float));
    
    vDSP_ctoz ( ( COMPLEX * ) a, 2, az, 1, length/2  );
    
    double t = getWallTime();
    double t2 = getCPUTime();
    
    /* -- BEGINNING OF BENCHMARKED SECTION -- */
    
    for(int i = 0; i < iter; i++) {
        vDSP_ctoz ( ( COMPLEX * ) a, 2, az, 1, length/2  );
        vDSP_fft_zrip ( fftSetup, az, 1, fftlog2, FFT_FORWARD  );
        vDSP_zvmul(az, 1, bz_global, 1, az, 1, length/2, 1);
        vDSP_fft_zrip ( fftSetup, az, 1, fftlog2, FFT_INVERSE  );
        vDSP_ztoc ( az, 1, ( COMPLEX * ) a, 2, length/2 );
    }
    
    /* -- END OF BENCHMARKED SECTION -- */
   
    t = getWallTime() - t;
    t2 = getCPUTime() - t2;
    vDSP_destroy_fftsetup(fftSetup);
    free(az->realp);
    free(az->imagp);
    free(az);
    free(a);
    
    benchmark b;
    b.fftsize = length;
    b.iterations = iter;
    b.name = "single threaded/vDSP";
    b.num_threads = 1;
    b.wallTime = t;
    b.cpuTime = t2;
    b.idleTime = t - t2;
    return b;
}





/* ----------------------- MULTI -THREADED IMPLEMENTATION WITH OpenMP ----------------------- */
#ifdef TEST_OMP
benchmark fft_vDSP_test_omp(int length, int iter, int user_num_threads) {
    int desired_num_threads;
#ifdef NUM_THREADS
    desired_num_threads = user_num_threads == -1 ? MIN(NUM_THREADS, iter) : MIN(user_num_threads, iter);
#else
    desired_num_threads = user_num_threads == -1 ? MIN(getNumCoresWithoutOmp(), iter) : MIN(user_num_threads, iter);
#endif
    iter = iter / desired_num_threads * desired_num_threads;
    
    vDSP_Length fftlog2 = (vDSP_Length)log2(length);
    FFTSetup fftSetup = vDSP_create_fftsetup(fftlog2, FFT_RADIX2);
    float* a = getRandomArray(length);
    DSPSplitComplex* az = malloc(sizeof(DSPSplitComplex));
    az->realp = malloc(length/2 * sizeof(float));
    az->imagp = malloc(length/2 * sizeof(float));
        
    int real_num_threads = 0;
        
    double t = getWallTime();
    double t2 = getCPUTime();
    
    /* -- BEGINNING OF BENCHMARKED SECTION -- */
#pragma omp parallel for num_threads(desired_num_threads) firstprivate(a, az)
    for(int i = 0; i < iter; i++) {
        int tid = omp_get_thread_num();
        if(tid==0) real_num_threads = omp_get_num_threads();
        vDSP_ctoz ( ( COMPLEX * ) a, 2, az, 1, length/2  );
        vDSP_fft_zrip ( fftSetup, az, 1, fftlog2, FFT_FORWARD  );
        vDSP_zvmul(az, 1, bz_global, 1, az, 1, length/2, 1);
        vDSP_fft_zrip ( fftSetup, az, 1, fftlog2, FFT_INVERSE  );
        vDSP_ztoc ( az, 1, ( COMPLEX * ) a, 2, length/2 );
    }
    /* -- END OF BENCHMARKED SECTION -- */
   
    t = getWallTime() - t;
    t2 = getCPUTime() - t2;
    
    vDSP_destroy_fftsetup(fftSetup);
    free(az->realp);
    free(az->imagp);
    free(az);
    free(a);
    
    benchmark b;
    b.fftsize = length;
    b.iterations = iter;
    b.name = "OpenMP/vDSP";
    b.num_threads = real_num_threads;
    b.wallTime = t;
    b.cpuTime = t2;
    b.idleTime = b.wallTime - b.cpuTime/real_num_threads;
    return b;
}
#endif


/* ----------------------- MULTI -THREADED IMPLEMENTATION WITH PTHREADS ----------------------- */

typedef struct fftStruct {
    FFTSetup fftSetup;
    int length;
    int iterations;
    vDSP_Length fftlog2;
    float *a;
    DSPSplitComplex *az;
} fftStruct;

fftStruct *fftStruct_new(int length) {
    fftStruct *obj = malloc(sizeof(fftStruct));
    obj->iterations = 0;
    obj->length = length;
    obj->fftlog2 = (vDSP_Length)log2(length);
    obj->fftSetup = vDSP_create_fftsetup(obj->fftlog2, FFT_RADIX2);
    obj->a = getRandomArray(length);
    obj->az = malloc(sizeof(DSPSplitComplex));
    obj->az->realp = malloc(length/2 * sizeof(float));
    obj->az->imagp = malloc(length/2 * sizeof(float));
    return obj;
}

void fftStruct_free(fftStruct *x) {
    vDSP_destroy_fftsetup(x->fftSetup);
    free(x->a);
    free(x->az->imagp);
    free(x->az->realp);
    free(x->az);
    free(x);
}

void* job(void *arg) {
    fftStruct *x = (fftStruct*)arg;
    for(int i = 0; i < x->iterations; i++) {
        vDSP_ctoz ( ( COMPLEX * ) x->a, 2, x->az, 1, x->length/2  );
        vDSP_fft_zrip ( x->fftSetup, x->az, 1, x->fftlog2, FFT_FORWARD  );
        vDSP_zvmul(x->az, 1, bz_global, 1, x->az, 1, x->length/2, 1);
        vDSP_fft_zrip ( x->fftSetup, x->az, 1, x->fftlog2, FFT_INVERSE  );
        vDSP_ztoc ( x->az, 1, ( COMPLEX * ) x->a, 2, x->length/2 );
    }

    pthread_exit(NULL);
}

benchmark fft_vDSP_test_pthread(int length, int iter, int user_num_threads) {
    int desired_num_threads;
#ifdef NUM_THREADS
    desired_num_threads = user_num_threads == -1 ? MIN(NUM_THREADS, iter) : MIN(user_num_threads, iter);
#else
    desired_num_threads = user_num_threads == -1 ? MIN(getNumCoresWithoutOmp(), iter) : MIN(user_num_threads, iter);
#endif
    iter = iter / desired_num_threads * desired_num_threads;
    
    pthread_t *threads = malloc(desired_num_threads * sizeof(pthread_t));
    
    fftStruct ** fftStructs = malloc(desired_num_threads * sizeof(fftStruct*));
    
    //TODO: what if iter is not a multiple of desired_num_threads?
    
    for(int i = 0; i < desired_num_threads; i++) {
        fftStructs[i] = fftStruct_new(length);
        fftStructs[i]->iterations = iter / desired_num_threads;
    }
    
    double t = getWallTime();
    double t2 = getCPUTime();
    
    /* -- BEGINNING OF BENCHMARKED SECTION -- */
    
    for(int i = 0; i < desired_num_threads; i++) {
        pthread_create(&threads[i], NULL, job, fftStructs[i]);
    }
    for(int i = 0; i < desired_num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    /* -- END OF BENCHMARKED SECTION -- */

    t = getWallTime() - t;
    t2 = getCPUTime() - t2;
    
    for(int i = 0; i < desired_num_threads; i++) {
        fftStruct_free(fftStructs[i]);
    }
    benchmark b;
    b.fftsize = length;
    b.iterations = iter;
    b.name = "pthreads/vDSP";
    b.num_threads = desired_num_threads;
    b.wallTime = t;
    b.cpuTime = t2;
    b.idleTime = b.wallTime - b.cpuTime/desired_num_threads;
    return b;
}
