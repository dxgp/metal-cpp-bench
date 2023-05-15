#include "MetalSaxpy.hpp"
#include <iostream>
#include <limits>


MetalSaxpy::MetalSaxpy(MTL::Device * device, uint64_t arrlen){
    arrayLength = arrlen;
    bufferSize = arrayLength * sizeof(float);
    _mDevice = device;
    NS::Error *error = nullptr; // if it fails

    MTL::Library *defaultLibrary = _mDevice->newDefaultLibrary();
    assert(defaultLibrary != nullptr);
    auto str = NS::String::string("saxpy", NS::ASCIIStringEncoding);
    MTL::Function *saxpyFunction = defaultLibrary->newFunction(str);
    defaultLibrary->release();
    assert(saxpyFunction!=nullptr);
    _mSaxpyFunctionPSO = _mDevice->newComputePipelineState(saxpyFunction, &error);
    saxpyFunction->release();
    assert(_mSaxpyFunctionPSO!=nullptr);
    _mCommandQueue = _mDevice->newCommandQueue();
    assert(_mCommandQueue != nullptr);
    _mX = _mDevice->newBuffer(bufferSize, MTL::ResourceStorageModeShared);
    _mY = _mDevice->newBuffer(bufferSize, MTL::ResourceStorageModeShared);
    _ma = _mDevice->newBuffer(sizeof(float), MTL::ResourceStorageModeShared);
    _mNumElems = _mDevice->newBuffer(sizeof(uint64_t), MTL::ResourceStorageModeShared);
    _mR = _mDevice->newBuffer(bufferSize, MTL::ResourceStorageModeShared);
    prepareData();
}

void MetalSaxpy::prepareData(){
    generateRandomFloatData(_mX);
    generateRandomFloatData(_mY);
    generateRandomFloatData(_ma);
    uint64_t *dataPtr = (uint64_t *)_mNumElems->contents();
    dataPtr[0] = arrayLength;
}

bool MetalSaxpy::areEqual(float a, float b) {
    return (fabs(a - b) <= FLT_EPSILON * std::max(1.0f, std::max(a, b)));
}

void MetalSaxpy::sendComputeCommand(){
    _mNumThreadsPerThreadgroup = _mSaxpyFunctionPSO->maxTotalThreadsPerThreadgroup();
    _mThreadsPerGrid = arrayLength;
    // std::cout<<"Max buffer length:"<<_mDevice->maxBufferLength()<<std::endl;
    MTL::CommandBuffer *commandBuffer = _mCommandQueue->commandBuffer();
    assert(commandBuffer != nullptr);
    MTL::ComputeCommandEncoder *computeEncoder = commandBuffer->computeCommandEncoder();
    assert(computeEncoder != nullptr);
    encodeSaxpyCommand(computeEncoder);
    computeEncoder->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
}

void MetalSaxpy::encodeSaxpyCommand(MTL::ComputeCommandEncoder *computeEncoder){

    computeEncoder->setComputePipelineState(_mSaxpyFunctionPSO);
    computeEncoder->setBuffer(_mX, 0, 0);
    computeEncoder->setBuffer(_mY, 0, 1);
    computeEncoder->setBuffer(_ma, 0, 2);
    computeEncoder->setBuffer(_mR, 0, 3);
    computeEncoder->setBuffer(_mNumElems, 0, 4);
    // if(AvlThreadGroupSize > arrayLength){
    //     AvlThreadGroupSize = arrayLength;
    // }
    // dispatchThreadgroups(threadgroupsPerGrid, threadsPerThreadgroup)
    computeEncoder->dispatchThreads(MTL::Size::Make(_mThreadsPerGrid, 1, 1), MTL::Size::Make(_mNumThreadsPerThreadgroup, 1, 1));
    // computeEncoder->dispatchThreadgroups(MTL::Size::Make(arrayLength/1024, 1, 1), MTL::Size::Make(_mNumThreadsPerThreadgroup, 1, 1));
}

void MetalSaxpy::generateRandomFloatData(MTL::Buffer *buffer){
    float *dataPtr = (float *)buffer->contents();
    for(uint64_t index = 0; index<buffer->length()/sizeof(float); index++){
        dataPtr[index] = (float)rand() / (float)(RAND_MAX);
    }
}

void MetalSaxpy::verifyResults(){
    float *x = (float *) _mX->contents();
    float *y = (float *) _mY->contents();
    float *result = (float *) _mR->contents();
    float *a = (float *) _ma->contents();
    for(uint64_t index = 0;index < arrayLength; index++){
        if(! areEqual(result[index], (x[index] * a[0]) + y[index])){
            std::cout<<"Computation Error Detected"<<std::endl;
            // std::cout<<"result[index] : "<<result[index]<<std::endl;
            // std::cout<<"(x[index] * a[0]) + y[index]"<<((x[index] * a[0]) + y[index])<<std::endl;
            // std::cout<<"x[index]: "<<x[index]<<std::endl;
            // std::cout<<"a[0] : "<<a[0]<<std::endl;
            // std::cout<<"y[index] : "<<y[index]<<std::endl;
            abort();
        }
    }
}

MetalSaxpy::~MetalSaxpy(){
    _mX->release();
    _mY->release();
    _mR->release();
    _ma->release();
    _mNumElems->release();
    _mSaxpyFunctionPSO->release();
    _mCommandQueue->release();
}