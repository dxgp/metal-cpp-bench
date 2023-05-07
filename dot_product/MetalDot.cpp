#include "MetalDot.hpp"
#include <iostream>
#include <limits>

MetalDot::MetalDot(MTL::Device *device){
    _mDevice = device;
    NS::Error *error = nullptr;
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
    _mR = _mDevice->newBuffer(sizeof(double), MTL::ResourceStorageModeShared);
    _mPartialSums = _mDevice->newBuffer(sizeof(float) * (_mDevice->maxThreadsPerThreadgroup()).width, MTL::ResourceStorageModeShared);
    _mNumElems = _mDevice->newBuffer(sizeof(uint64_t), MTL::ResourceStorageModeShared);
    // _mThreadPositions = _mDevice->newBuffer(sizeof(unsigned int)*arrayLength, MTL::ResourceStorageModeShared);
    // _mCounter = _mDevice->newBuffer(sizeof(unsigned int), MTL::ResourceStorageModeShared);
    prepareData();
}

void MetalDot::prepareData(){
    generateRandomFloatData(_mX);
    generateRandomFloatData(_mY);
    uint64_t *dataptr = (uint64_t *)_mNumElems->contents();
    dataptr[0] = arrayLength;
}

void MetalDot::sendComputeCommand(int num_metal_threadgroups_per_grid, int num_groups_per_grid){
    _mNumThreadsPerGroup = num_metal_threadgroups_per_grid;
    _mNumGroupsPerGrid = num_groups_per_grid;
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
    // computeEncoder->setBuffer(_mThreadPositions, 0, 5);
    // computeEncoder->setBuffer(_mCounter, 0, 6);
    // unsigned int *Z = (unsigned int *) _mCounter->contents();
    // Z[0] = 0;
    MTL::Size gridSize = MTL::Size::Make(arrayLength, 1, 1);
    MTL::Size threadsPerGrid = MTL::Size::Make(1024, 1, 1);
    // MTL::Size threadsPerThreadGroup = MTL::Size::Make(1024, 1, 1);
    computeEncoder->dispatchThreads(gridSize, threadsPerGrid);
}

bool MetalDot::areEqual(float a, float b) {
    return (fabs(a - b) <= FLT_EPSILON * std::max(1.0f, std::max(a, b)));
}

void MetalDot::verifyResults(){
    float *X = (float *) _mX->contents();
    float *Y = (float *) _mY->contents();
    float *R = (float *) _mR->contents();
    float *partial_sums = (float *) _mPartialSums->contents();

    // unsigned int *thread_positions = (unsigned int *) _mThreadPositions->contents();
    float partialSum = 0;
    for(uint64_t i = 0; i<arrayLength;i++){
        partialSum += X[i] * Y[i];
    }
    float partialMetal = 0;
    std::cout<<"Partial Sums : [";
    for(uint64_t i = 0; i<_mPartialSums->length()/sizeof(float); i++){
        partialMetal += partial_sums[i];
        if(i!= _mPartialSums->length()/sizeof(float) - 1){
            std::cout<<partial_sums[i]<<",";
        } else{
            std::cout<<partial_sums[i]<<"]"<<std::endl;
        }
    }
    // GETTING THREAD POSITIONS IS UTTERLY USELESS
    // std::cout<<"Thread Positions : [";
    // int threadpos[arrayLength];
    // for(uint64_t i = 0; i<arrayLength; i++){
    //     threadpos[i] = thread_positions[i];
    // }
    // std::sort(threadpos, threadpos + arrayLength);
    // for(uint64_t i = 0; i<arrayLength; i++){
    //     if(i!=arrayLength - 1){
    //         std::cout<<threadpos[i]<<",";
    //     } else{
    //         std::cout<<threadpos[i]<<"]"<<std::endl;
    //     }
    // }
    if(!areEqual(partialSum, partialMetal)){
        std::cout<<"Incorrect computation"<<std::endl;
        std::cout<<"Predicted Result: "<<partialSum<<std::endl;
        std::cout<<"Computed Result: "<<partialMetal<<std::endl;
        abort();
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