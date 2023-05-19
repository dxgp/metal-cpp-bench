#include <metal_stdlib>

using namespace metal;

kernel void dot(
    device const float*     X[[buffer(0)]],
    device const float*     Y[[buffer(1)]],
    device       float*     R[[buffer(2)]],
    device       uint64_t*   num_elements[[buffer(3)]],
    device       float*     partial_sums[[buffer(4)]],
    const uint thread_position_in_grid [[thread_position_in_grid]],
    const uint threads_per_grid         [[threads_per_grid]]
){
    float sum = 0;
    sum += X[thread_position_in_grid] * Y[thread_position_in_grid];
    threadgroup_barrier( mem_flags::mem_device );
    partial_sums[thread_position_in_grid] = sum;
}