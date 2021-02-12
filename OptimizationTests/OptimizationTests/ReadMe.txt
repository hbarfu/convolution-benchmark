NOTE: It is DISCOURAGED to disable OMP testing since OMP's omp_get_wtime() function really measures wall clock time, while clock_gettime(CLOCK_MONOTONIC, ...) seems to not count time spend in suspend mode, even though it should.


You can install OpenMP in the shell with:
brew install libomp

---------------------------------------------------

Usage as command line tool:
./OptimizationTests fftSize numConvolutions numIterations (numThreads)

---------------------------------------------------


IF YOU HAVE TROUBLE COMPILING:

add to Header Search paths: usr/local/include
add to Library Search paths: usr/local/lib


----------------------------------------------------

IF YOU HAVE TROUBLE COMPLILING FFTW:

linker flags for fftw3_threads:
OTHER_LDFLAGS = -lfftw3_threads -lfftw3 -lm

(replace fftw3 with fftw3f in all paths if you want to use single precision)

---------------------------------------------------

IF YOU HAVE TROUBLE COMPILING OMP:

linker flags for fftw with omp:
OTHER_LDFLAGS = -lfftw3_omp -lfftw3 -lm -lomp

"Other C flags" for fftw3 with omp:
OTHER_CFLAGS = -Xclang -fopenmp

(replace fftw3 with fftw3f in all paths if you want to use single precision)

---------------------------------------------------
