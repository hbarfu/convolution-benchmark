//
//  fft_fftw.c
//  OptimizationTests
//
//  Created by Hannes on 05.02.21.
//  Copyright © 2021 hbarfu. All rights reserved.
//

#include "fft_fftw.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "omp.h"

#define NTHREADS 4

void fft_fftw_test(int length, int iter) {
    //fftwf_init_threads();
    //fftwf_plan_with_nthreads(NTHREADS);
    
    fftwf_complex *in, *out;
    fftwf_plan p;
    fftwf_plan pi;
    in = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * length);
    out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * length);
    p = fftwf_plan_dft_1d(length, in, out, FFTW_FORWARD, FFTW_PATIENT);
    pi = fftwf_plan_dft_1d(length, in, out, FFTW_BACKWARD, FFTW_PATIENT);
    
    //printf("starting ffts\n");
    
    double t = omp_get_wtime();

    
    for(int i = 0; i < iter; i++) {
        fftwf_execute(p); /* repeat as needed */
        fftwf_execute(pi);
    }
    
    t = omp_get_wtime() - t;
    printf("FFTW time: %fms\n@%d iterations of %dpt FFT\n", t*1000, iter, length);
    
    /*for(int i = 0; i < length; i++) {
        printf("%f, ", *(out + i));
    }
    printf("\n");*/
    
    fftwf_destroy_plan(p);
    fftwf_free(in); fftwf_free(out);
}
