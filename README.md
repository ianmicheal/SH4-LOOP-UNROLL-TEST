# SH4-LOOP-UNROLL-TEST
Target the best amount to unroll loops
	Description: loop unroll test for the sh4 cpu 
	Idea based on pcercuei/sh4_gcc_test_unroll.c
 # 🎮 Dreamcast SH4 Color Conversion Benchmark

A high-performance benchmark suite for BGR555 to RGB565 color conversion optimized for the Sega Dreamcast's SH4 CPU. This tool helps developers find the fastest pixel format conversion method for their Dreamcast homebrew projects.

![Dreamcast](https://img.shields.io/badge/Platform-Dreamcast-blue)
![SH4](https://img.shields.io/badge/CPU-SH4-green)
![KOS](https://img.shields.io/badge/SDK-KallistiOS-orange)
![License](https://img.shields.io/badge/License-MIT-yellow)

## 🚀 Features

- **17 Different Optimization Techniques** - From basic 16-bit operations to advanced cache-optimized implementations
- **Real-world Performance Metrics** - Shows MB/s throughput and estimated FPS for 640x480 displays
- **SH4-Specific Optimizations** - Leverages Dreamcast hardware features like prefetch instructions
- **Clear Recommendations** - Tells you exactly which approach to use 
- **Automatic Verification** - Ensures all conversions are pixel-perfect

## 📊 What It Tests

The benchmark evaluates various approaches to converting pixels from BGR555 (used by some video sources) to RGB565 (Dreamcast's native format):

### Bit-width Strategies
- **16-bit** - Process 1 pixel at a time
- **32-bit** - Process 2 pixels at once  
- **64-bit** - Process 4 pixels at once

### Loop Unrolling Factors
- No unrolling (1x)
- Moderate unrolling (2x, 4x)
- Heavy unrolling (8x, 16x)

### Advanced Techniques
- Cache prefetching
- SIMD-style parallel processing
- Cache-line aligned operations
- Register scheduling optimization

## 🛠️ Building

### Prerequisites
- [KallistiOS (KOS)](http://gamedev.allusion.net/softprj/kos/) development environment
- Dreamcast toolchain (sh-elf-gcc)
- Make

### Compilation
```bash
make
test running
OUTPUT:> BGR555 to RGB565 Conversion Benchmark for Dreamcast SH4
OUTPUT:> ========================================================
OUTPUT:> Buffer size: 524288 pixels (1024 KB)
OUTPUT:> Running 5 iterations per test
OUTPUT:> Verifying conversion correctness... [OK] All conversions correct!
OUTPUT:> Running tests...
OUTPUT:> ----------------
OUTPUT:> Test  0: 16-bit single            completed
OUTPUT:> Test  1: 16-bit unroll 2          completed
OUTPUT:> Test  2: 16-bit unroll 4          completed
OUTPUT:> Test  3: 16-bit unroll 8          completed
OUTPUT:> Test  4: 32-bit single            completed
OUTPUT:> Test  5: 32-bit unroll 2          completed
OUTPUT:> Test  6: 32-bit unroll 4          completed
OUTPUT:> Test  7: 32-bit unroll 8          completed
OUTPUT:> Test  8: 64-bit single            completed
OUTPUT:> Test  9: 64-bit unroll 2          completed
OUTPUT:> Test 10: 64-bit unroll 4          completed
OUTPUT:> Test 11: 64-bit unroll 8          completed
OUTPUT:> Test 12: 16-bit unroll 16         completed
OUTPUT:> Test 13: 32-bit unroll 16         completed
OUTPUT:> Test 14: Prefetch + 32-bit x8     completed
OUTPUT:> Test 15: SIMD-style 32-bit        completed
OUTPUT:> Test 16: Cache-optimized 32-bit   completed
OUTPUT:> ========== RESULTS SUMMARY ==========
OUTPUT:> *** WINNER: Test 15 (SIMD-style 32-bit) ***
OUTPUT:>     Time: 30725 microseconds
OUTPUT:>     Speed: 32.5 MB/s
OUTPUT:> PERFORMANCE BREAKDOWN:
OUTPUT:> ----------------------
OUTPUT:> 16-BIT OPERATIONS (processing 1 pixel at a time):
OUTPUT:>   [SLOW]       Test  0: 16-bit single              5.09x slower
OUTPUT:>   [SLOW]       Test  1: 16-bit unroll 2            3.00x slower
OUTPUT:>   [SLOW]       Test  2: 16-bit unroll 4            2.15x slower
OUTPUT:>   [SLOW]       Test  3: 16-bit unroll 8            3.53x slower
OUTPUT:>   [SLOW]       Test 12: 16-bit unroll 16           3.47x slower
OUTPUT:> 32-BIT OPERATIONS (processing 2 pixels at once):
OUTPUT:>   [SLOW]       Test  4: 32-bit single              2.58x slower
OUTPUT:>   [SLOW]       Test  5: 32-bit unroll 2            1.52x slower
OUTPUT:>   [SLOW]       Test  6: 32-bit unroll 4            2.08x slower
OUTPUT:>   [SLOW]       Test  7: 32-bit unroll 8            2.36x slower
OUTPUT:>   [SLOW]       Test 13: 32-bit unroll 16           1.85x slower
OUTPUT:> 64-BIT OPERATIONS (processing 4 pixels at once):
OUTPUT:>   [SLOW]       Test  8: 64-bit single              1.73x slower
OUTPUT:>   [SLOW]       Test  9: 64-bit unroll 2            1.58x slower
OUTPUT:>   [SLOW]       Test 10: 64-bit unroll 4            2.20x slower
OUTPUT:>   [SLOW]       Test 11: 64-bit unroll 8            2.23x slower
OUTPUT:> ADVANCED TECHNIQUES:
OUTPUT:>   [SLOW]       Test 14: Prefetch + 32-bit x8       2.36x slower
OUTPUT:>   [EXCELLENT]  Test 15: SIMD-style 32-bit          1.00x slower
OUTPUT:>   [SLOW]       Test 16: Cache-optimized 32-bit     1.59x slower
OUTPUT:> >>> RECOMMENDATIONS FOR DREAMCAST DEVELOPERS <<<
OUTPUT:> ==============================================
OUTPUT:> 1. OPTIMAL APPROACH: Use advanced techniques
OUTPUT:>    - Cache prefetching or SIMD-style processing wins
OUTPUT:>    - Requires more complex code but gives best performance
OUTPUT:> 2. LOOP UNROLLING: Heavy unrolling (8x) is beneficial
OUTPUT:>    - Maximum instruction-level parallelism
OUTPUT:>    - Good for the SH4's pipeline
OUTPUT:> 3. AVOID THESE:
OUTPUT:>    X 16-bit operations - too slow for production use
OUTPUT:>    X 64-bit with heavy unrolling - diminishing returns
OUTPUT:> 4. FOR YOUR CODE:
OUTPUT:>    -> Copy the approach from Test 15
OUTPUT:>    -> Process data in 64-bit chunks
OUTPUT:>    -> Align your buffers to 32-byte boundaries
OUTPUT:> 5. EXPECTED PERFORMANCE:
OUTPUT:>    - Best case: 32.5 MB/s (56 frames/sec for 640x480)
OUTPUT:>    - This is 5.1x faster than naive 16-bit approach
OUTPUT:> ============================================
OUTPUT:> --- RAW PERFORMANCE DATA ---
OUTPUT:> ----------------------------
OUTPUT:> Test  0: 156324 us     6.4 MB/s   5.09x
OUTPUT:> Test  1:  92198 us    10.8 MB/s   3.00x
OUTPUT:> Test  2:  65962 us    15.2 MB/s   2.15x
OUTPUT:> Test  3: 108336 us     9.2 MB/s   3.53x
OUTPUT:> Test  4:  79247 us    12.6 MB/s   2.58x
OUTPUT:> Test  5:  46597 us    21.5 MB/s   1.52x
OUTPUT:> Test  6:  63910 us    15.6 MB/s   2.08x
OUTPUT:> Test  7:  72522 us    13.8 MB/s   2.36x
OUTPUT:> Test  8:  53217 us    18.8 MB/s   1.73x
OUTPUT:> Test  9:  48621 us    20.6 MB/s   1.58x
OUTPUT:> Test 10:  67608 us    14.8 MB/s   2.20x
OUTPUT:> Test 11:  68390 us    14.6 MB/s   2.23x
OUTPUT:> Test 12: 106734 us     9.4 MB/s   3.47x
OUTPUT:> Test 13:  56927 us    17.6 MB/s   1.85x
OUTPUT:> Test 14:  72527 us    13.8 MB/s   2.36x
OUTPUT:> Test 15:  30725 us    32.5 MB/s   1.00x
OUTPUT:> Test 16:  48973 us    20.4 MB/s   1.59x
OUTPUT:> arch: exit return code 0
OUTPUT:> arch: shutting down kernel
OUTPUT:> maple: final stats -- device count = 22, vbl_cntr = 489, dma_cntr = 440
OUTPUT:> vid_set_mode: 640x480 VGA with 1 framebuffers.
STATE:> Upload processus completed on 6/17/2025 - 8:12:22 PM, Exit Code : 0
