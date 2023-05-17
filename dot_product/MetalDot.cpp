#include "MetalDot.hpp"
#include <iostream>
#include <limits>


MetalDot::MetalDot(MTL::Device *device, uint64_t arrlen){
    arrayLength = arrlen;
    bufferSize = arrayLength * sizeof(float);
    _mDevice = device;
    NS::Error *error = nullptr;
    // function to capture device frame metal
    
    MTL::Library *defaultLibrary = _mDevice->newDefaultLibrary();
    assert(defaultLibrary != nullptr);
    auto str = NS::String::string("dot", NS::ASCIIStringEncoding);
    MTL::Function *dotFunction = defaultLibrary->newFunction(str);
    defaultLibrary->release();
    assert(dotFunction!=nullptr);
    _mDotFunctionPSO = _mDevice->newComputePipelineState(dotFunction, &error);
    dotFunction->release();
    assert(_mDotFunctionPSO!=nullptr);
    _mCommandQueue = _mDevice->newCommandQueue();
    assert(_mCommandQueue!=nullptr);
    _mX = _mDevice->newBuffer(bufferSize, MTL::ResourceStorageModeShared);
    _mY = _mDevice->newBuffer(bufferSize, MTL::ResourceStorageModeShared);
    _mR = _mDevice->newBuffer(sizeof(float), MTL::ResourceStorageModeShared);
    //_mPartialSums = _mDevice->newBuffer(sizeof(float) * (_mDevice->maxThreadsPerThreadgroup()).width, MTL::ResourceStorageModeShared);
    _mPartialSums = _mDevice->newBuffer(arrayLength*sizeof(float), MTL::ResourceStorageModeShared);
    _mNumElems = _mDevice->newBuffer(sizeof(uint64_t), MTL::ResourceStorageModeShared);
    prepareData();
}

void MetalDot::prepareData(){
    generateRandomFloatData(_mX);
    generateRandomFloatData(_mY);
    uint64_t *dataptr = (uint64_t *)_mNumElems->contents();
    dataptr[0] = arrayLength;
}

void MetalDot::sendComputeCommand(){
    _mNumThreadsPerThreadgroup = _mDotFunctionPSO->maxTotalThreadsPerThreadgroup();
    _mThreadsPerGrid = arrayLength;
    MTL::CommandBuffer *commandBuffer = _mCommandQueue->commandBuffer();
    assert(commandBuffer!=nullptr);
    MTL::ComputeCommandEncoder *computeEncoder = commandBuffer->computeCommandEncoder();
    assert(computeEncoder!=nullptr);
    encodeDotCommand(computeEncoder);
    computeEncoder->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
}

void MetalDot::encodeDotCommand(MTL::ComputeCommandEncoder *computeEncoder){
    computeEncoder->setComputePipelineState(_mDotFunctionPSO);
    computeEncoder->setBuffer(_mX, 0, 0);
    computeEncoder->setBuffer(_mY, 0, 1);
    computeEncoder->setBuffer(_mR, 0, 2);
    computeEncoder->setBuffer(_mNumElems, 0, 3);
    computeEncoder->setBuffer(_mPartialSums, 0, 4);
    computeEncoder->dispatchThreads(MTL::Size::Make(_mThreadsPerGrid, 1, 1), MTL::Size::Make(_mNumThreadsPerThreadgroup, 1, 1));
}

bool MetalDot::areEqual(float a, float b) {
    return (fabs(a - b) <= FLT_EPSILON * std::max(1.0f, std::max(a, b)));
}

void MetalDot::verifyResults(){
    float *X = (float *) _mX->contents();
    float *Y = (float *) _mY->contents();
    float *R = (float *) _mR->contents();
    float *partial_sums = (float *) _mPartialSums->contents();

    // std::cout<<"Made it here"<<std::endl;
    float partialSum = 0;
    for(uint64_t i = 0; i<arrayLength;i++){
        partialSum += X[i] * Y[i];
    }
    float partialMetal = 0;
    for(uint64_t i = 0; i<_mPartialSums->length()/sizeof(float); i++){
        partialMetal += partial_sums[i];
    }
    if(!areEqual(partialSum, partialMetal)){
        std::cout<<"Incorrect computation"<<std::endl;
        std::cout<<"Predicted Result: "<<partialSum<<std::endl;
        std::cout<<"Computed Result: "<<partialMetal<<std::endl;
        abort();
    } else{
        std::cout<<"Computation verified!"<<std::endl;
    }
}

void MetalDot::generateRandomFloatData(MTL::Buffer *buffer){
    float *dataptr = (float *)buffer->contents();
    for(uint64_t index = 0; index<buffer->length()/sizeof(float);index++){
        dataptr[index] = ((float)rand() / float(RAND_MAX))*10;
    }
}
MetalDot::~MetalDot(){
    _mX->release();
    _mY->release();
    _mR->release();
    _mNumElems->release();
    _mDotFunctionPSO->release();
    _mCommandQueue->release();   
}