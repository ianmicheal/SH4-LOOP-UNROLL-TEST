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
- **Clear Recommendations** - Tells you exactly which approach to use in plain English
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
