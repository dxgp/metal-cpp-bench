#include <metal_stdlib>

using namespace metal;

constant int tileSize = 32; // Tile size for tiling


kernel void matrixMultiply(
                const device float* A [[buffer(0)]],
                const device float* B [[buffer(1)]],
                device float* C [[buffer(2)]],
                const device uint* N [[buffer(3)]],
                uint2 blockDim [[threads_per_threadgroup]],
                uint2 threadIdx [[thread_position_in_threadgroup]],
                uint2 blockIdx [[threadgroup_position_in_grid]]
){
    threadgroup float tileA[tileSize][tileSize];
    threadgroup float tileB[tileSize][tileSize];
    uint n = N[0];
    uint row = blockDim.y*blockIdx.y + threadIdx.y;
    uint col = blockDim.x*blockIdx.x + threadIdx.x;
    float cvalue = 0.0;
    tileA[threadIdx.y][threadIdx.x] = 0;
    tileB[threadIdx.y][threadIdx.x] = 0;
    for(uint k=0; k<((n-1)/tileSize)+1 ;k++){
        if(row<n && (threadIdx.x + (k*tileSize)) < n){
            tileA[threadIdx.y][threadIdx.x] = A[row*n + threadIdx.x + (k*tileSize)];
        }
        else{
            tileA[threadIdx.y][threadIdx.x] = 0;
        }
        if(col<n && (threadIdx.y + (k*tileSize)) < n){
            tileB[threadIdx.y][threadIdx.x] = B[(threadIdx.y + (k*tileSize))*n + col];
        }
        else{
            tileB[threadIdx.y][threadIdx.x] = 0;
        }
        threadgroup_barrier(mem_flags::mem_threadgroup);
        for(int i=0; i<tileSize; i++){
            cvalue += tileA[threadIdx.y][i] * tileB[i][threadIdx.x];
        }
    }
    if(row<n && col<n){
        C[row*n + col] = cvalue;
    }
}