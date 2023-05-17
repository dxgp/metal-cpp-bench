#include <metal_stdlib>

using namespace metal;

kernel void hadamard_product(
    device const float* A [[ buffer(0) ]],
    device const float* B [[ buffer(1) ]],
    device float* C [[ buffer(2) ]],
    uint gid [[ thread_position_in_grid ]]
){
    C[gid] = A[gid] * B[gid];
}