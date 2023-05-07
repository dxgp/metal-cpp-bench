#pragma once

#include "metal-cpp/Foundation/Foundation.hpp"
#include "metal-cpp/Metal/Metal.hpp"

// unsigned int arrayLength = 60*180*10000; //same as num elements
// unsigned int bufferSize = arrayLength * sizeof(float);


class MetalSaxpy{
    public: 
    uint64_t arrayLength = 60*180*10000; //same as num elements
    uint64_t bufferSize = arrayLength * sizeof(float);
    MTL::Device * _mDevice;
    MTL::ComputePipelineState *_mSaxpyFunctionPSO;
    MTL::CommandQueue * _mCommandQueue;

    MTL::Buffer *_mX;
    MTL::Buffer *_mY;
    MTL::Buffer *_ma;
    MTL::Buffer *_mR;
    MTL::Buffer *_mNumElems;

    int _mNumGroupsPerGrid;
    int _mNumThreadsPerGroup;

    MetalSaxpy(MTL::Device *device);
    ~MetalSaxpy();

    bool areEqual(float a, float b);
    void prepareData();
    void sendComputeCommand(int num_metal_threadgroups_per_grid, int num_groups_per_grid);
    void verifyResults();

    private:
    void encodeSaxpyCommand(MTL::ComputeCommandEncoder *computeEncoder);
    void generateRandomFloatData(MTL::Buffer *buffer);
};

