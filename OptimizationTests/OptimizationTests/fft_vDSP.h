//
//  fft_vDSP.h
//  OptimizationTests
//
//  Created by Hannes on 05.02.21.
//  Copyright © 2021 hbarfu. All rights reserved.
//

#ifndef fft_vDSP_h
#define fft_vDSP_h

#include <stdio.h>
#include <Accelerate/Accelerate.h>
#include "util.h"

DSPSplitComplex *makeBZ(int length);
benchmark fft_vDSP_test(int length, int iter);
benchmark fft_vDSP_test_omp(int length, int iter, int user_num_threads);
benchmark fft_vDSP_test_pthread(int length, int iter, int user_num_threads);

#endif /* fft_vDSP_h */
