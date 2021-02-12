//
//  util.c
//  OptimizationTests
//
//  Created by Hannes on 05.02.21.
//  Copyright © 2021 hbarfu. All rights reserved.
//

#include "defs.h"
#include "util.h"
#include <stdlib.h>
#include <sys/utsname.h>
#include <sys/sysctl.h>
#include <mach/mach_time.h>
#ifdef TEST_OMP
#include "omp.h"
#endif

unsigned int getNumCoresWithoutOmp(void)
{
  size_t len;
  unsigned int ncpu;

  len = sizeof(ncpu);
  sysctlbyname ("hw.logicalcpu",&ncpu,&len,NULL,0);

  return ncpu;
}

double getWallTime(void) {
    #ifdef TEST_OMP
        return omp_get_wtime();
    #else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ((double) (1000000000 * ts.tv_sec + ts.tv_nsec)) / 1000000000;
        /*struct timeval tv;
        struct timezone tz;
        gettimeofday(&tv, &tz);
        return ((double) (1000000 * tv.tv_sec + tv.tv_usec)) / 1000000;*/
    #endif
}

double getCPUTime(void) {
    struct rusage r;
    getrusage(RUSAGE_SELF, &r);
    return ((double) (1000000 * r.ru_utime.tv_sec + r.ru_utime.tv_usec)) / 1000000;
    /*struct timespec ts;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
    return ((double) (1000000000 * ts.tv_sec + ts.tv_nsec)) / 1000000000;*/
}

void printDeviceInfo() {
    struct utsname deviceInfo;
    uname(&deviceInfo);
    printf("%s\n%s\n%s\n%s\n%s\n", deviceInfo.machine, deviceInfo.nodename, deviceInfo.release, deviceInfo.sysname, deviceInfo.version);
}

float* getRandomArray(int n) {
    float* a = malloc(n * sizeof(float));
    for(int i = 0; i < n; i++) {
        a[i] = (float)(rand()/(float)(RAND_MAX/2.0f)) -1.0f;
    }
    return a;
}

DSPSplitComplex * getRandomComplexArray(int n) {
    DSPSplitComplex *a = malloc(sizeof(DSPSplitComplex));
    a->imagp = malloc(n/2*sizeof(float));
    a->realp = malloc(n/2*sizeof(float));
    for(int i = 0; i < n/2; i++) {
        a->realp[i] = (float)(rand()/(float)(RAND_MAX/2.0f)) -1.0f;
        a->imagp[i] = (float)(rand()/(float)(RAND_MAX/2.0f)) -1.0f;
    }
    return a;
}
