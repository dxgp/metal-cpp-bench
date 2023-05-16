#include <metal_stdlib>


using namespace metal;

kernel void denseMV(

    device const float*           M [[ buffer(0) ]],
    device       float*           X [[ buffer(1) ]],
    device       float*           R [[ buffer(2) ]],
    device       uint64_t*           MatSize [[ buffer(3) ]],
    device       uint64_t *             num_elements [[ buffer(4) ]],

    const        uint             thread_position_in_grid [[ thread_position_in_grid ]],
    const        uint             threads_per_grid        [[ threads_per_grid ]]
) {
    // for ( uint64_t i = thread_position_in_grid; i < num_elements[0]; i += threads_per_grid ) {
    
    //     R[i] = X[i] * a + Y[i];
    // }
    int colIndex = thread_position_in_grid % MatSize[1];
    int rowIndex = (thread_position_in_grid - colIndex) / MatSize[1];
    R[rowIndex] += M[thread_position_in_grid] * X[colIndex];
    simdgroup_barrier(mem_flags::mem_threadgroup);
}