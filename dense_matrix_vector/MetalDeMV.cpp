#include "MetalDeMV.hpp"
#include <iostream>
#include <algorithm>
#include <random>
#include <string>
void printMTLBuffer(MTL::Buffer *buf, std::string name){
    std::cout<<name<<":"<<"<";
    float *ptr = (float*)buf->contents();
    for(int i=0;i<buf->length()/sizeof(float) - 1;i++){
        std::cout<<ptr[i]<<",";
    }
    std::cout<<ptr[buf->length()/sizeof(float) - 1]<<">";
    std::cout<<std::endl;
}
void printVector(float *vec, uint64_t length){
    std::cout<<"<";
    for(int i=0;i<length - 1;i++){
        std::cout<<vec[i]<<",";
    }
    std::cout<<vec[length - 1]<<">";
    std::cout<<std::endl;
}
MetalDeMV::MetalDeMV(MTL::Device *device, uint64_t nrows, uint64_t ncols){
    uint64_t bufferLengthMatrix = nrows*ncols*sizeof(float);
    uint64_t bufferLengthVector = ncols*sizeof(float);
    _mDevice = device;
    numRows = nrows;
    numCols = ncols;
    NS::Error *error = nullptr;
    // create the default library
    MTL::Library* lib = _mDevice->newDefaultLibrary();
    assert(lib!=nullptr);
    MTL::Function *demv = lib->newFunction(NS::String::string("denseMV",NS::ASCIIStringEncoding));
    lib->release();
    assert(demv!=nullptr);
    _mComputePSO = _mDevice->newComputePipelineState(demv, &error);
    demv->release();
    assert(_mComputePSO!=nullptr);
    _mCommandQueue = _mDevice->newCommandQueue();
    assert(_mCommandQueue!=nullptr);
    _mM = _mDevice->newBuffer(bufferLengthMatrix, MTL::ResourceStorageModeShared);
    _mX = _mDevice->newBuffer(bufferLengthVector, MTL::ResourceStorageModeShared);
    _mR = _mDevice->newBuffer(bufferLengthVector, MTL::ResourceStorageModeShared);
    _mMatSize = _mDevice->newBuffer(sizeof(uint64_t)*2, MTL::ResourceStorageModeShared);
    // data generation
    prepareData();
}
void MetalDeMV::prepareData(){
    genRandomMatrix(numRows, numCols, 0.7, _mM);
    genRandomMatrix(numCols, 1, 1, _mX);
    uint64_t *matsize = (uint64_t*)_mMatSize->contents();
    matsize[0] = numRows;
    matsize[1] = numCols;
}
void MetalDeMV::sendComputeCommand(){
    
    MTL::CommandBuffer *commandBuffer = _mCommandQueue->commandBuffer();
    assert(commandBuffer!=nullptr);
    MTL::ComputeCommandEncoder *computeEncoder = commandBuffer->computeCommandEncoder();
    assert(computeEncoder!=nullptr);
    encodeDeMVCommand(computeEncoder);
    computeEncoder->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
    verifyResults();
}
void MetalDeMV::verifyResults(){
    float *m = (float*)_mM->contents();
    float *x = (float*)_mX->contents();
    float *r = (float*)_mR->contents();
    std::vector<float> res;
    res.resize(numCols);
    for(int i=0;i<numRows*numCols; i++){
        int colIndex = i%numCols;
        int rowIndex = (i - colIndex)/numCols;
        res[rowIndex] += m[i]*x[colIndex];
    }
    printVector(res.data(), res.size());
}
void MetalDeMV::encodeDeMVCommand(MTL::ComputeCommandEncoder *computeEncoder){
    computeEncoder->setComputePipelineState(_mComputePSO);
    computeEncoder->setBuffer(_mM, 0, 0);
    computeEncoder->setBuffer(_mX, 0, 1);
    computeEncoder->setBuffer(_mR, 0, 2);
    computeEncoder->setBuffer(_mMatSize, 0, 3);
    int maxRoof = 0;
    std::cout<<"Execution Width:"<<_mComputePSO->threadExecutionWidth()<<std::endl;
    std::cout<<"Execution Width:"<<_mDevice->
    if((numRows*numCols)<_mComputePSO->maxTotalThreadsPerThreadgroup()){
        maxRoof = numRows*numCols;
    } else{
        maxRoof = _mComputePSO->maxTotalThreadsPerThreadgroup();
    }
    computeEncoder->dispatchThreads(MTL::Size::Make(numRows, numCols, 1), MTL::Size::Make(1, 1, 1));
}



bool MetalDeMV::areEqual(float a, float b) {
    return (fabs(a - b) <= FLT_EPSILON * std::max(1.0f, std::max(a, b)));
}
void MetalDeMV::genRandomMatrix(uint64_t nrows, uint64_t ncols, float density, MTL::Buffer* buffer){
    uint64_t dataLen = nrows*ncols;
    float *data = (float *)buffer->contents();
    // warm up rand
    std::random_device rd;
    std::mt19937 g(rd());
    g();
    g();
    for(uint64_t i=0;i<dataLen;i++){
        if(i<dataLen*density){
            data[i] = (float)rand() / (float)(RAND_MAX);
        } else{
            data[i] = 0;
        }
    }
    if(density!=1){
        std::shuffle(&data[0], &data[dataLen - 1], g);
    }
    std::cout<<"<";
    for(int i=0;i<dataLen- 1;i++){
        std::cout<<data[i]<<",";
    }
    std::cout<<data[dataLen - 1]<<">"<<std::endl;
}
