/*

    This program is designed to gather the performance data of the POWER10
    GEMM kernels in `blis/sandbox/power10`.

    By default, the performance of the kernels is gather over a set of square
    matrices. The perfromance results are reported in GFLOPS, and outputted in
    CSV format.

*/

#include "performance.h"
#include "blis.h"
#include "common.h"
#include "gemm_template.h"

#include <stdio.h>
// print kernel name
const char* get_kernel_name(int kernel_id)
{
    switch (kernel_id)
    {
        case FLOAT16 : return "bli_shgemm";
        case BFLOAT16: return "bli_sbgemm";
        case INT16   : return "bli_i16gemm";
        case INT8    : return "bli_i8gemm";
        case INT4    : return "bli_i4gemm";
        default: printf("INCORRECT KERNEL ID\n"); exit(-1);
    }
}

// set defaults for gemm kernel generation
#ifndef PREFIX
#define PREFIX sh_
#endif
#ifndef DTIN
#define DTIN float16
#endif
#ifndef DTOUT
#define DTOUT float
#endif
#ifndef UKERNEL_ITER
#define UKERNEL_ITER (pb/2 + pb%2) 
#endif
#ifndef PACK_A_FUNC
#define PACK_A_FUNC sh_pack_a
#endif
#ifndef PACK_B_FUNC
#define PACK_B_FUNC sh_pack_b
#endif
#ifndef UKERNEL_NAME
#define UKERNEL_NAME bli_shgemm_power10_mma_8x16
#endif
#ifndef OUTER_PRODS
#define OUTER_PRODS 2
#endif
#ifndef MR
#define MR 8
#endif
#ifndef NR
#define NR 16
#endif
#ifndef MC
#define MC 384
#endif
#ifndef KC
#define KC 3384
#endif
#ifndef NC
#define NC 3072
#endif
#ifndef AALIGNBUF
#define AALIGNBUF 0
#endif
#ifndef BALIGNBUF
#define BALIGNBUF 0
#endif





// the pack kernels come from BLIS
// cache params (MC, KC, NC) are passed as macros in the makefile
GENERIC_GEMM(
    PREFIX, // kernel name prefix 
    DTIN, // input type
    DTOUT, // output type    
    UKERNEL_ITER, // innermost loop iterations
    PACK_A_FUNC, // pack kernel for A
    PACK_B_FUNC, // pack kernel for B
    UKERNEL_NAME, // microkernel function name
    OUTER_PRODS, // K_MMA
    MR, // MR
    NR, // NR
    MC, // MC
    KC, // KC
    NC, // NC
    AALIGNBUF, // A_ALIGN
    BALIGNBUF // B_ALIGN
);


// create all the performance gathering functions for each kernel
GET_PERF_API_TEMP(sh, bli_sh_gemm, float16, float);
//GET_PERF_API_TEMP(sb, bli_sbgemm, bfloat16, float);
//GET_PERF_API_TEMP(i16, bli_i16gemm, int16_t, int);
//GET_PERF_API_TEMP(i8, bli_i8gemm, int8_t, int);
//GET_PERF_API_TEMP(i4, bli_i4gemm, nibbles, int);


// using the DATATYPE enum, gather the performance of the respective GEMM kernel
double run_kernel(int kernel_id, int nreps, int m, int n, int k)
{
    switch (kernel_id)
    {
        case FLOAT16 : return test_shapi(nreps, m, n, k);
       // case BFLOAT16: return test_sbapi(nreps, m, n, k);
       // case INT16   : return test_i16api(nreps, m, n, k);
       // case INT8    : return test_i8api(nreps, m, n, k);
       // case INT4    : return test_i4api(nreps, m, n, k);
        default: return -1.0;
    }
}

// print the performance data in CSV format
// performance is measured in terms of GFLOPs
void print_perf_data(int m, int n, int k, double best_time)
{
    double GFLOPS = (2.0 * m * n * k) / (1e9 * best_time);
    printf("%d, %d, %d, %.2f\n", m, n, k, GFLOPS);
}

// get performance data
void get_perf(int kernel_id, int nreps, int start, int end, int inc)
{
    // csv header
    printf("%s performance\n", get_kernel_name(kernel_id));
    printf("MC = %d, KC = %d, NC = %d\n", MC, KC, NC);
    printf("m, n, k, GFLOPS\n");

    int m,n,k;

    // run over all problem sizes
    for (int p=start; p<=end; p+=inc)
    {
        // change here to adjust problem size
        m = MC*6,
        n = NC*2,
        k = p;

        double best_run_time = run_kernel(kernel_id, nreps, m, n, k);

        print_perf_data(m, n, k, best_run_time);
    }
}

int main(int argc, char *argv[])
{
    // initialize a square problem set range
    int start = 80;
    int end = 4000;
    int inc = 80;
    
    // number of times the kernel will be run
    // only the best time is stored
    int nreps = 10;

    // run a respective kernel
    get_perf( FLOAT16, nreps, start, end, inc);
//    get_perf(BFLOAT16, nreps, start, end, inc);
//    get_perf(   INT16, nreps, start, end, inc);
//    get_perf(    INT8, nreps, start, end, inc);
//    get_perf(    INT4, nreps, start, end, inc);

    return 0;
}
