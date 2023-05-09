#pragma once

#include "metal-cpp/Foundation/Foundation.hpp"
#include "metal-cpp/Metal/Metal.hpp"

class MetalDot{
    public:
    uint64_t arrayLength = 2048;
    uint64_t bufferSize = arrayLength*sizeof(float);
    MTL::Device *_mDevice;
    MTL::ComputePipelineState *_mDotFunctionPSO;
    MTL::CommandQueue * _mCommandQueue;

    MTL::Buffer *_mX;
    MTL::Buffer *_mY;
    MTL::Buffer *_mR;
    MTL::Buffer *_mNumElems;
    MTL::Buffer *_mPartialSums; 

    int _mNumGroupsPerGrid;
    int _mNumThreadsPerGroup;

    MetalDot(MTL::Device *device);
    ~MetalDot();
    bool areEqual(float a, float b);
    void prepareData();
    void sendComputeCommand(int num_metal_threadgroups_per_grid, int num_groups_per_grid);
    void verifyResults();

    private:
    void encodeDotCommand(MTL::ComputeCommandEncoder *computeEncoder);
    void generateRandomFloatData(MTL::Buffer *buffer);

};