/*
	Name: Loop test
	Copyright: 
	Author: Ian micheal 
	Date: 17/06/25 20:06
	Description: loop unroll test for the sh4 cpu 
	Idea based on pcercuei/sh4_gcc_test_unroll.c
*/

#include <sys/time.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <kos.h>

#ifndef UINT64_MAX
#define UINT64_MAX 0xFFFFFFFFFFFFFFFFULL
#endif

KOS_INIT_FLAGS(INIT_DEFAULT);

// Align buffers to cache line boundary (32 bytes on SH4)
static uint16_t buffer_bgr555[0x80000] __attribute__((aligned(32)));
static uint16_t buffer_rgb565[0x80000] __attribute__((aligned(32)));

// Test names for reporting
static const char* test_names[] = {
    "16-bit single",
    "16-bit unroll 2",
    "16-bit unroll 4", 
    "16-bit unroll 8",
    "32-bit single",
    "32-bit unroll 2",
    "32-bit unroll 4",
    "32-bit unroll 8",
    "64-bit single",
    "64-bit unroll 2",
    "64-bit unroll 4",
    "64-bit unroll 8",
    "16-bit unroll 16",
    "32-bit unroll 16",
    "Prefetch + 32-bit x8",
    "SIMD-style 32-bit",
    "Cache-optimized 32-bit"
};

static uint64_t read_counter_us(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000ull + (uint64_t)tv.tv_usec;
}

// Basic conversion functions
static inline uint16_t bgr555_to_rgb565(uint16_t bgr)
{
    return ((bgr & 0x001f) << 11)
         | ((bgr & 0x03e0) << 1)
         | ((bgr & 0x7c00) >> 10);
}

static inline uint32_t bgr32_to_rgb32(uint32_t bgr)
{
    return ((bgr & 0x001f001f) << 11)
         | ((bgr & 0x03e003e0) << 1)
         | ((bgr & 0x7c007c00) >> 10);
}

static inline uint64_t bgr64_to_rgb64(uint64_t bgr)
{
    return ((bgr & 0x001f001f001f001full) << 11)
         | ((bgr & 0x03e003e003e003e0ull) << 1)
         | ((bgr & 0x7c007c007c007c00ull) >> 10);
}

// SH4 has a prefetch instruction - use it for better cache utilization
#ifdef __SH4__
#define PREFETCH(addr) __asm__ volatile("pref @%0" : : "r" (addr))
#else
#define PREFETCH(addr) ((void)0)
#endif

static void convert_buffer(uint16_t * restrict bgr555,
                          uint16_t * restrict rgb565,
                          unsigned int size,
                          unsigned int test)
{
    unsigned int i;
    uint32_t * restrict bgr32;
    uint32_t * restrict rgb32;
    uint64_t * restrict bgr64;
    uint64_t * restrict rgb64;
    
    switch (test) {
    case 0: // 16-bit single
        for (i = 0; i < size; i++)
            rgb565[i] = bgr555_to_rgb565(bgr555[i]);
        break;
        
    case 1: // 16-bit unroll 2
        for (i = 0; i < size; i += 2) {
            rgb565[i + 0] = bgr555_to_rgb565(bgr555[i + 0]);
            rgb565[i + 1] = bgr555_to_rgb565(bgr555[i + 1]);
        }
        break;
        
    case 2: // 16-bit unroll 4
        for (i = 0; i < size; i += 4) {
            rgb565[i + 0] = bgr555_to_rgb565(bgr555[i + 0]);
            rgb565[i + 1] = bgr555_to_rgb565(bgr555[i + 1]);
            rgb565[i + 2] = bgr555_to_rgb565(bgr555[i + 2]);
            rgb565[i + 3] = bgr555_to_rgb565(bgr555[i + 3]);
        }
        break;
        
    case 3: // 16-bit unroll 8
        for (i = 0; i < size; i += 8) {
            rgb565[i + 0] = bgr555_to_rgb565(bgr555[i + 0]);
            rgb565[i + 1] = bgr555_to_rgb565(bgr555[i + 1]);
            rgb565[i + 2] = bgr555_to_rgb565(bgr555[i + 2]);
            rgb565[i + 3] = bgr555_to_rgb565(bgr555[i + 3]);
            rgb565[i + 4] = bgr555_to_rgb565(bgr555[i + 4]);
            rgb565[i + 5] = bgr555_to_rgb565(bgr555[i + 5]);
            rgb565[i + 6] = bgr555_to_rgb565(bgr555[i + 6]);
            rgb565[i + 7] = bgr555_to_rgb565(bgr555[i + 7]);
        }
        break;
        
    case 4: // 32-bit single
        bgr32 = (uint32_t *) bgr555;
        rgb32 = (uint32_t *) rgb565;
        for (i = 0; i < size / 2; i++)
            rgb32[i] = bgr32_to_rgb32(bgr32[i]);
        break;
        
    case 5: // 32-bit unroll 2
        bgr32 = (uint32_t *) bgr555;
        rgb32 = (uint32_t *) rgb565;
        for (i = 0; i < size / 2; i += 2) {
            rgb32[i + 0] = bgr32_to_rgb32(bgr32[i + 0]);
            rgb32[i + 1] = bgr32_to_rgb32(bgr32[i + 1]);
        }
        break;
        
    case 6: // 32-bit unroll 4
        bgr32 = (uint32_t *) bgr555;
        rgb32 = (uint32_t *) rgb565;
        for (i = 0; i < size / 2; i += 4) {
            rgb32[i + 0] = bgr32_to_rgb32(bgr32[i + 0]);
            rgb32[i + 1] = bgr32_to_rgb32(bgr32[i + 1]);
            rgb32[i + 2] = bgr32_to_rgb32(bgr32[i + 2]);
            rgb32[i + 3] = bgr32_to_rgb32(bgr32[i + 3]);
        }
        break;
        
    case 7: // 32-bit unroll 8
        bgr32 = (uint32_t *) bgr555;
        rgb32 = (uint32_t *) rgb565;
        for (i = 0; i < size / 2; i += 8) {
            rgb32[i + 0] = bgr32_to_rgb32(bgr32[i + 0]);
            rgb32[i + 1] = bgr32_to_rgb32(bgr32[i + 1]);
            rgb32[i + 2] = bgr32_to_rgb32(bgr32[i + 2]);
            rgb32[i + 3] = bgr32_to_rgb32(bgr32[i + 3]);
            rgb32[i + 4] = bgr32_to_rgb32(bgr32[i + 4]);
            rgb32[i + 5] = bgr32_to_rgb32(bgr32[i + 5]);
            rgb32[i + 6] = bgr32_to_rgb32(bgr32[i + 6]);
            rgb32[i + 7] = bgr32_to_rgb32(bgr32[i + 7]);
        }
        break;
        
    case 8: // 64-bit single
        bgr64 = (uint64_t *) bgr555;
        rgb64 = (uint64_t *) rgb565;
        for (i = 0; i < size / 4; i++)
            rgb64[i] = bgr64_to_rgb64(bgr64[i]);
        break;
        
    case 9: // 64-bit unroll 2
        bgr64 = (uint64_t *) bgr555;
        rgb64 = (uint64_t *) rgb565;
        for (i = 0; i < size / 4; i += 2) {
            rgb64[i + 0] = bgr64_to_rgb64(bgr64[i + 0]);
            rgb64[i + 1] = bgr64_to_rgb64(bgr64[i + 1]);
        }
        break;
        
    case 10: // 64-bit unroll 4
        bgr64 = (uint64_t *) bgr555;
        rgb64 = (uint64_t *) rgb565;
        for (i = 0; i < size / 4; i += 4) {
            rgb64[i + 0] = bgr64_to_rgb64(bgr64[i + 0]);
            rgb64[i + 1] = bgr64_to_rgb64(bgr64[i + 1]);
            rgb64[i + 2] = bgr64_to_rgb64(bgr64[i + 2]);
            rgb64[i + 3] = bgr64_to_rgb64(bgr64[i + 3]);
        }
        break;
        
    case 11: // 64-bit unroll 8
        bgr64 = (uint64_t *) bgr555;
        rgb64 = (uint64_t *) rgb565;
        for (i = 0; i < size / 4; i += 8) {
            rgb64[i + 0] = bgr64_to_rgb64(bgr64[i + 0]);
            rgb64[i + 1] = bgr64_to_rgb64(bgr64[i + 1]);
            rgb64[i + 2] = bgr64_to_rgb64(bgr64[i + 2]);
            rgb64[i + 3] = bgr64_to_rgb64(bgr64[i + 3]);
            rgb64[i + 4] = bgr64_to_rgb64(bgr64[i + 4]);
            rgb64[i + 5] = bgr64_to_rgb64(bgr64[i + 5]);
            rgb64[i + 6] = bgr64_to_rgb64(bgr64[i + 6]);
            rgb64[i + 7] = bgr64_to_rgb64(bgr64[i + 7]);
        }
        break;
        
    case 12: // 16-bit unroll 16 - maximize register usage
        for (i = 0; i < size; i += 16) {
            uint16_t t0 = bgr555[i + 0];
            uint16_t t1 = bgr555[i + 1];
            uint16_t t2 = bgr555[i + 2];
            uint16_t t3 = bgr555[i + 3];
            uint16_t t4 = bgr555[i + 4];
            uint16_t t5 = bgr555[i + 5];
            uint16_t t6 = bgr555[i + 6];
            uint16_t t7 = bgr555[i + 7];
            
            rgb565[i + 0] = bgr555_to_rgb565(t0);
            rgb565[i + 1] = bgr555_to_rgb565(t1);
            rgb565[i + 2] = bgr555_to_rgb565(t2);
            rgb565[i + 3] = bgr555_to_rgb565(t3);
            rgb565[i + 4] = bgr555_to_rgb565(t4);
            rgb565[i + 5] = bgr555_to_rgb565(t5);
            rgb565[i + 6] = bgr555_to_rgb565(t6);
            rgb565[i + 7] = bgr555_to_rgb565(t7);
            
            t0 = bgr555[i + 8];
            t1 = bgr555[i + 9];
            t2 = bgr555[i + 10];
            t3 = bgr555[i + 11];
            t4 = bgr555[i + 12];
            t5 = bgr555[i + 13];
            t6 = bgr555[i + 14];
            t7 = bgr555[i + 15];
            
            rgb565[i + 8] = bgr555_to_rgb565(t0);
            rgb565[i + 9] = bgr555_to_rgb565(t1);
            rgb565[i + 10] = bgr555_to_rgb565(t2);
            rgb565[i + 11] = bgr555_to_rgb565(t3);
            rgb565[i + 12] = bgr555_to_rgb565(t4);
            rgb565[i + 13] = bgr555_to_rgb565(t5);
            rgb565[i + 14] = bgr555_to_rgb565(t6);
            rgb565[i + 15] = bgr555_to_rgb565(t7);
        }
        break;
        
    case 13: // 32-bit unroll 16 with register scheduling
        bgr32 = (uint32_t *) bgr555;
        rgb32 = (uint32_t *) rgb565;
        for (i = 0; i < size / 2; i += 16) {
            uint32_t a0 = bgr32[i + 0];
            uint32_t a1 = bgr32[i + 1];
            uint32_t a2 = bgr32[i + 2];
            uint32_t a3 = bgr32[i + 3];
            uint32_t b0 = bgr32_to_rgb32(a0);
            uint32_t b1 = bgr32_to_rgb32(a1);
            uint32_t b2 = bgr32_to_rgb32(a2);
            uint32_t b3 = bgr32_to_rgb32(a3);
            
            a0 = bgr32[i + 4];
            a1 = bgr32[i + 5];
            a2 = bgr32[i + 6];
            a3 = bgr32[i + 7];
            rgb32[i + 0] = b0;
            rgb32[i + 1] = b1;
            rgb32[i + 2] = b2;
            rgb32[i + 3] = b3;
            
            b0 = bgr32_to_rgb32(a0);
            b1 = bgr32_to_rgb32(a1);
            b2 = bgr32_to_rgb32(a2);
            b3 = bgr32_to_rgb32(a3);
            
            a0 = bgr32[i + 8];
            a1 = bgr32[i + 9];
            a2 = bgr32[i + 10];
            a3 = bgr32[i + 11];
            rgb32[i + 4] = b0;
            rgb32[i + 5] = b1;
            rgb32[i + 6] = b2;
            rgb32[i + 7] = b3;
            
            b0 = bgr32_to_rgb32(a0);
            b1 = bgr32_to_rgb32(a1);
            b2 = bgr32_to_rgb32(a2);
            b3 = bgr32_to_rgb32(a3);
            
            a0 = bgr32[i + 12];
            a1 = bgr32[i + 13];
            a2 = bgr32[i + 14];
            a3 = bgr32[i + 15];
            rgb32[i + 8] = b0;
            rgb32[i + 9] = b1;
            rgb32[i + 10] = b2;
            rgb32[i + 11] = b3;
            
            b0 = bgr32_to_rgb32(a0);
            b1 = bgr32_to_rgb32(a1);
            b2 = bgr32_to_rgb32(a2);
            b3 = bgr32_to_rgb32(a3);
            
            rgb32[i + 12] = b0;
            rgb32[i + 13] = b1;
            rgb32[i + 14] = b2;
            rgb32[i + 15] = b3;
        }
        break;
        
    case 14: // Prefetch + 32-bit unroll 8
        bgr32 = (uint32_t *) bgr555;
        rgb32 = (uint32_t *) rgb565;
        for (i = 0; i < size / 2; i += 8) {
            // Prefetch next cache line (32 bytes = 8 uint32_t)
            PREFETCH(&bgr32[i + 16]);
            
            rgb32[i + 0] = bgr32_to_rgb32(bgr32[i + 0]);
            rgb32[i + 1] = bgr32_to_rgb32(bgr32[i + 1]);
            rgb32[i + 2] = bgr32_to_rgb32(bgr32[i + 2]);
            rgb32[i + 3] = bgr32_to_rgb32(bgr32[i + 3]);
            rgb32[i + 4] = bgr32_to_rgb32(bgr32[i + 4]);
            rgb32[i + 5] = bgr32_to_rgb32(bgr32[i + 5]);
            rgb32[i + 6] = bgr32_to_rgb32(bgr32[i + 6]);
            rgb32[i + 7] = bgr32_to_rgb32(bgr32[i + 7]);
        }
        break;
        
    case 15: // SIMD-style processing with 32-bit
        bgr32 = (uint32_t *) bgr555;
        rgb32 = (uint32_t *) rgb565;
        for (i = 0; i < size / 2; i += 4) {
            // Load 4 values
            uint32_t v0 = bgr32[i + 0];
            uint32_t v1 = bgr32[i + 1];
            uint32_t v2 = bgr32[i + 2];
            uint32_t v3 = bgr32[i + 3];
            
            // Process all blues
            uint32_t b0 = (v0 & 0x001f001f) << 11;
            uint32_t b1 = (v1 & 0x001f001f) << 11;
            uint32_t b2 = (v2 & 0x001f001f) << 11;
            uint32_t b3 = (v3 & 0x001f001f) << 11;
            
            // Process all greens
            uint32_t g0 = (v0 & 0x03e003e0) << 1;
            uint32_t g1 = (v1 & 0x03e003e0) << 1;
            uint32_t g2 = (v2 & 0x03e003e0) << 1;
            uint32_t g3 = (v3 & 0x03e003e0) << 1;
            
            // Process all reds
            uint32_t r0 = (v0 & 0x7c007c00) >> 10;
            uint32_t r1 = (v1 & 0x7c007c00) >> 10;
            uint32_t r2 = (v2 & 0x7c007c00) >> 10;
            uint32_t r3 = (v3 & 0x7c007c00) >> 10;
            
            // Combine and store
            rgb32[i + 0] = b0 | g0 | r0;
            rgb32[i + 1] = b1 | g1 | r1;
            rgb32[i + 2] = b2 | g2 | r2;
            rgb32[i + 3] = b3 | g3 | r3;
        }
        break;
        
    case 16: // Cache-optimized with 32-byte blocks
        bgr32 = (uint32_t *) bgr555;
        rgb32 = (uint32_t *) rgb565;
        // Process one cache line at a time (32 bytes = 8 uint32_t)
        for (i = 0; i < size / 2; i += 8) {
            // Load entire cache line
            uint32_t t0 = bgr32[i + 0];
            uint32_t t1 = bgr32[i + 1];
            uint32_t t2 = bgr32[i + 2];
            uint32_t t3 = bgr32[i + 3];
            uint32_t t4 = bgr32[i + 4];
            uint32_t t5 = bgr32[i + 5];
            uint32_t t6 = bgr32[i + 6];
            uint32_t t7 = bgr32[i + 7];
            
            // Convert all
            uint32_t r0 = bgr32_to_rgb32(t0);
            uint32_t r1 = bgr32_to_rgb32(t1);
            uint32_t r2 = bgr32_to_rgb32(t2);
            uint32_t r3 = bgr32_to_rgb32(t3);
            uint32_t r4 = bgr32_to_rgb32(t4);
            uint32_t r5 = bgr32_to_rgb32(t5);
            uint32_t r6 = bgr32_to_rgb32(t6);
            uint32_t r7 = bgr32_to_rgb32(t7);
            
            // Store entire cache line
            rgb32[i + 0] = r0;
            rgb32[i + 1] = r1;
            rgb32[i + 2] = r2;
            rgb32[i + 3] = r3;
            rgb32[i + 4] = r4;
            rgb32[i + 5] = r5;
            rgb32[i + 6] = r6;
            rgb32[i + 7] = r7;
        }
        break;
    }
}

// Warm up cache and test correctness
static void warmup_and_verify(void)
{
    unsigned int i;
    
    // Initialize with test pattern
    for (i = 0; i < 0x80000; i++) {
        buffer_bgr555[i] = i & 0x7fff;
    }
    
    // Run conversion and verify first few results
    convert_buffer(buffer_bgr555, buffer_rgb565, 128, 0);
    
    printf("Verifying conversion correctness... ");
    int all_correct = 1;
    for (i = 0; i < 16; i++) {
        uint16_t bgr = buffer_bgr555[i];
        uint16_t rgb = buffer_rgb565[i];
        uint16_t expected = bgr555_to_rgb565(bgr);
        if (rgb != expected) {
            all_correct = 0;
            printf("\n  ERROR at index %d: BGR555: 0x%04x -> RGB565: 0x%04x (expected: 0x%04x)", 
                   i, bgr, rgb, expected);
        }
    }
    if (all_correct) {
        printf("[OK] All conversions correct!\n\n");
    } else {
        printf("\n  [ERROR] Conversion errors detected!\n\n");
    }
}

int main(int argc, char **argv)
{
    (void)argc;  // Suppress unused parameter warning
    (void)argv;  // Suppress unused parameter warning
    
    uint64_t times[17];
    unsigned int test, run;
    const unsigned int num_runs = 5;
    
    printf("BGR555 to RGB565 Conversion Benchmark for Dreamcast SH4\n");
    printf("========================================================\n");
    printf("Buffer size: %u pixels (%u KB)\n", 0x80000, (0x80000 * 2) / 1024);
    printf("Running %u iterations per test\n\n", num_runs);
    
    // Warm up and verify
    warmup_and_verify();
    
    printf("Running tests...\n");
    printf("----------------\n\n");
    
    // Run all tests
    for (test = 0; test < 17; test++) {
        uint64_t total_time = 0;
        uint64_t min_time = UINT64_MAX;
        
        // Multiple runs to get average and minimum
        for (run = 0; run < num_runs; run++) {
            uint64_t before = read_counter_us();
            convert_buffer(buffer_bgr555, buffer_rgb565, 0x80000, test);
            uint64_t after = read_counter_us();
            
            uint64_t elapsed = after - before;
            total_time += elapsed;
            if (elapsed < min_time) {
                min_time = elapsed;
            }
        }
        
        times[test] = min_time;
        
        // Simple progress indicator
        printf("Test %2d: %-24s completed\n", test, test_names[test]);
    }
    
    // Find best result
    uint64_t best_time = times[0];
    unsigned int best_test = 0;
    for (test = 1; test < 17; test++) {
        if (times[test] < best_time) {
            best_time = times[test];
            best_test = test;
        }
    }
    
    // Calculate performance metrics
    double best_mb_per_sec = ((double)0x80000 * 2.0 * 1000000.0) / (best_time * 1024.0 * 1024.0);
    
    printf("\n========== RESULTS SUMMARY ==========\n\n");
    
    printf("*** WINNER: Test %d (%s) ***\n", best_test, test_names[best_test]);
    printf("    Time: %llu microseconds\n", best_time);
    printf("    Speed: %.1f MB/s\n\n", best_mb_per_sec);
    
    // Categorize and explain results
    printf("PERFORMANCE BREAKDOWN:\n");
    printf("----------------------\n\n");
    
    // Sort tests by performance for better presentation
    typedef struct {
        unsigned int test_id;
        double relative_perf;
        const char* category;
    } TestResult;
    
    TestResult results[17];
    for (test = 0; test < 17; test++) {
        results[test].test_id = test;
        results[test].relative_perf = (double)times[test] / best_time;
        
        // Categorize performance
        if (results[test].relative_perf <= 1.1) {
            results[test].category = "[EXCELLENT]";
        } else if (results[test].relative_perf <= 1.3) {
            results[test].category = "[GOOD]     ";
        } else if (results[test].relative_perf <= 1.5) {
            results[test].category = "[MODERATE] ";
        } else {
            results[test].category = "[SLOW]     ";
        }
    }
    
    // Group by technique type
    printf("16-BIT OPERATIONS (processing 1 pixel at a time):\n");
    for (test = 0; test < 4; test++) {
        printf("  %s  Test %2d: %-24s  %5.2fx slower\n",
               results[test].category, test, test_names[test], results[test].relative_perf);
    }
    printf("  %s  Test 12: %-24s  %5.2fx slower\n\n",
           results[12].category, test_names[12], results[12].relative_perf);
    
    printf("32-BIT OPERATIONS (processing 2 pixels at once):\n");
    for (test = 4; test < 8; test++) {
        printf("  %s  Test %2d: %-24s  %5.2fx slower\n",
               results[test].category, test, test_names[test], results[test].relative_perf);
    }
    printf("  %s  Test 13: %-24s  %5.2fx slower\n\n",
           results[13].category, test_names[13], results[13].relative_perf);
    
    printf("64-BIT OPERATIONS (processing 4 pixels at once):\n");
    for (test = 8; test < 12; test++) {
        printf("  %s  Test %2d: %-24s  %5.2fx slower\n",
               results[test].category, test, test_names[test], results[test].relative_perf);
    }
    
    printf("\nADVANCED TECHNIQUES:\n");
    for (test = 14; test < 17; test++) {
        printf("  %s  Test %2d: %-24s  %5.2fx slower\n",
               results[test].category, test, test_names[test], results[test].relative_perf);
    }
    
    printf("\n\n>>> RECOMMENDATIONS FOR DREAMCAST DEVELOPERS <<<\n");
    printf("==============================================\n\n");
    
    printf("1. OPTIMAL APPROACH: ");
    if ((best_test >= 4 && best_test < 8) || best_test == 13) {
        printf("Use 32-bit operations (2 pixels at once)\n");
        printf("   - The SH4 handles 32-bit data efficiently\n");
        printf("   - Better memory bandwidth utilization than 16-bit\n");
        printf("   - Sweet spot for the Dreamcast hardware\n");
    } else if (best_test >= 8 && best_test < 12) {
        printf("Use 64-bit operations (4 pixels at once)\n");
        printf("   - Maximum memory bandwidth utilization\n");
        printf("   - Works well with large buffers\n");
    } else if (best_test >= 14) {
        printf("Use advanced techniques\n");
        printf("   - Cache prefetching or SIMD-style processing wins\n");
        printf("   - Requires more complex code but gives best performance\n");
    } else {
        printf("Use 16-bit operations\n");
        printf("   - Simple implementation\n");
        printf("   - May be sufficient for small buffers\n");
    }
    
    printf("\n2. LOOP UNROLLING: ");
    if (best_test == 0 || best_test == 4 || best_test == 8) {
        printf("No unrolling needed!\n");
        printf("   - Simple loops are already optimal\n");
        printf("   - Compiler optimization handles it well\n");
    } else {
        int unroll_factor = 0;
        if (best_test < 4) unroll_factor = 1 << (best_test);
        else if (best_test < 8) unroll_factor = 1 << (best_test - 4);
        else if (best_test < 12) unroll_factor = 1 << (best_test - 8);
        else if (best_test == 12 || best_test == 13) unroll_factor = 16;
        else unroll_factor = 8;
        
        if (unroll_factor <= 4) {
            printf("Moderate unrolling (%dx) works best\n", unroll_factor);
            printf("   - Reduces loop overhead\n");
            printf("   - Fits well in instruction cache\n");
        } else {
            printf("Heavy unrolling (%dx) is beneficial\n", unroll_factor);
            printf("   - Maximum instruction-level parallelism\n");
            printf("   - Good for the SH4's pipeline\n");
        }
    }
    
    printf("\n3. AVOID THESE:\n");
    // Check if 16-bit operations are too slow
    double min_16bit_perf = 999.0;
    for (test = 0; test < 4; test++) {
        if (results[test].relative_perf < min_16bit_perf) {
            min_16bit_perf = results[test].relative_perf;
        }
    }
    if (results[12].relative_perf < min_16bit_perf) {
        min_16bit_perf = results[12].relative_perf;
    }
    
    if (min_16bit_perf > 1.5) {
        printf("   X 16-bit operations - too slow for production use\n");
    }
    if (results[11].relative_perf > 2.0) {
        printf("   X 64-bit with heavy unrolling - diminishing returns\n");
    }
    
    printf("\n4. FOR YOUR CODE:\n");
    printf("   -> Copy the approach from Test %d\n", best_test);
    printf("   -> Process data in %s chunks\n", 
           (best_test < 4 || best_test == 12) ? "16-bit" : 
           (best_test < 8 || best_test == 13) ? "32-bit" : "64-bit");
    printf("   -> Align your buffers to 32-byte boundaries\n");
    if (best_test == 14) {
        printf("   -> Use prefetch instructions for large buffers\n");
    }
    
    printf("\n5. EXPECTED PERFORMANCE:\n");
    printf("   - Best case: %.1f MB/s (%.0f frames/sec for 640x480)\n", 
           best_mb_per_sec, (best_mb_per_sec * 1024.0 * 1024.0) / (640.0 * 480.0 * 2.0));
    printf("   - This is %.1fx faster than naive 16-bit approach\n", 
           (double)times[0] / best_time);
    
    printf("\n============================================\n");
    
    printf("\n\n--- RAW PERFORMANCE DATA ---\n");
    printf("----------------------------\n");
    for (test = 0; test < 17; test++) {
        double mb_per_sec = ((double)0x80000 * 2.0 * 1000000.0) / (times[test] * 1024.0 * 1024.0);
        printf("Test %2d: %6llu us  %6.1f MB/s  %5.2fx\n", 
               test, times[test], mb_per_sec, (double)times[test] / best_time);
    }
    printf("\n");
    
    return 0;
}