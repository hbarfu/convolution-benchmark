//
//  ViewController.swift
//  OptimizationTests_iOS
//
//  Created by Hannes on 09.02.21.
//  Copyright © 2021 hbarfu. All rights reserved.
//

import UIKit

class ViewController: UIViewController {
    
    //these are still defined in defs.h!
    let test_iterations:Int32 = TEST_ITERATIONS
    let num_convolutions:Int32 = NUM_FFTS_PER_TEST
    let length:Int32 = LENGTH
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        print("starting benchmarks...\n\n")
        
        makeBZ(Int32(length));
        
        var b_vdsp:benchmark = benchmark(name: nil, wallTime: 0, cpuTime: 0, idleTime: 0, num_threads: 0, iterations: 0, fftsize: 0);
        var b1:benchmark;
        var b_pthread:benchmark = benchmark(name: nil, wallTime: 0, cpuTime: 0, idleTime: 0, num_threads: 0, iterations: 0, fftsize: 0);
        var b2:benchmark;
        
        for _ in 0...test_iterations {
            b1 = fft_vDSP_test(Int32(length), Int32(num_convolutions));
            b_vdsp.fftsize = b1.fftsize; b_vdsp.iterations = b1.iterations; b_vdsp.name = b1.name; b_vdsp.num_threads = b1.num_threads; b_vdsp.wallTime += b1.wallTime; b_vdsp.cpuTime += b1.cpuTime; b_vdsp.idleTime += b1.idleTime;
        }
        
        for _ in 0...test_iterations {
            b2 = fft_vDSP_test_pthread(Int32(length), Int32(num_convolutions), -1);
            b_pthread.fftsize = b2.fftsize; b_pthread.iterations = b2.iterations; b_pthread.name = b2.name; b_pthread.num_threads = b2.num_threads; b_pthread.wallTime += b2.wallTime; b_pthread.cpuTime += b2.cpuTime; b_pthread.idleTime += b2.idleTime;
        }
        
        b_vdsp.wallTime = b_vdsp.wallTime / Double(test_iterations);
        b_vdsp.cpuTime = b_vdsp.cpuTime / Double(test_iterations);
        b_vdsp.idleTime = b_vdsp.idleTime / Double(test_iterations);
        b_pthread.wallTime = b_pthread.wallTime / Double(test_iterations);
        b_pthread.cpuTime = b_pthread.cpuTime / Double(test_iterations);
        b_pthread.idleTime = b_pthread.idleTime / Double(test_iterations);
        
        print("⏱ \(String(cString: b_vdsp.name)) time: \nexecution time: \(b_vdsp.wallTime*1000)ms\ncpu time: \(b_vdsp.cpuTime*1000)ms\nidle time: \(b_vdsp.idleTime*1000)ms\n\t@\(b_vdsp.iterations) executions of \(b_vdsp.fftsize)pt FFT spread on \(b_vdsp.num_threads) threads\n\n")
        print("⏱ \(String(cString: b_pthread.name)) time: \nexecution time: \(b_pthread.wallTime*1000)ms\ntotal cpu time: \(b_pthread.cpuTime*1000)ms\nidle time per thread: \(b_pthread.idleTime*1000)ms\n\t@\(b_pthread.iterations) executions of \(b_pthread.fftsize)pt FFT spread on \(b_pthread.num_threads) threads\n\n")

    }


}

