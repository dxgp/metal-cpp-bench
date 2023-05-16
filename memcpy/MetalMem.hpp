#pragma once

#include "metal-cpp/Foundation/Foundation.hpp"
#include "metal-cpp/Metal/Metal.hpp"

class MetalMem{
    public:
    uint64_t arrayLength;
    uint64_t bufferLength;
    MTL::Device* _mDevice;
    MTL::ComputePipelineState* _mComputePSO;
    MTL::CommandQueue* _mCommandQueue;
    MTL::Buffer* _mIn; // the matrix is here
    MTL::Buffer* _mOut; // the vector is here

    MetalMem(MTL::Device* device, uint64_t arrlen);
    bool areEqual(float a, float b);
    void prepareData();
    void sendComputeCommand();
    void verifyResults();
    private:
    void encodeMemCommand(MTL::ComputeCommandEncoder *computeEncoder);
    void generateRandomFloatData(MTL::Buffer *buffer);
};