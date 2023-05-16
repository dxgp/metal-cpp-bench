#include <metal_stdlib>


using namespace metal;

kernel void saxpy(

    device const float*           X [[ buffer(0) ]],
    device       float*           Y [[ buffer(1) ]],
    device       float&           a [[ buffer(2) ]],
    device       float*           R [[ buffer(3) ]],
    device       uint64_t *       num_elements [[ buffer(4) ]],
    const        uint             tpig [[ thread_position_in_grid ]],
    const        uint             threads_per_grid        [[ threads_per_grid ]]
) {
        R[tpig] = X[tpig] * a + Y[tpig];
}