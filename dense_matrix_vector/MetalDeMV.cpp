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
    matData = new float[nrows*ncols];
    _mDevice = device;
    numRows = nrows;
    numCols = ncols;
    NS::Error *error = nullptr;
    // create the default library
    MTL::Library* lib = _mDevice->newDefaultLibrary();
    assert(lib!=nullptr);
    MTL::Function *demv = lib->newFunction(NS::String::string("dot",NS::ASCIIStringEncoding));
    lib->release();
    assert(demv!=nullptr);
    _mComputePSO = _mDevice->newComputePipelineState(demv, &error);
    demv->release();
    assert(_mComputePSO!=nullptr);
    _mCommandQueue = _mDevice->newCommandQueue();
    assert(_mCommandQueue!=nullptr);
    //_mM = _mDevice->newBuffer(bufferLengthMatrix, MTL::ResourceStorageModeShared);
    for(int i=0;i<numRows;i++){
        _mMatBuffers.push_back(_mDevice->newBuffer(bufferLengthVector, MTL::ResourceStorageModeShared));
    }
    _mX = _mDevice->newBuffer(bufferLengthVector, MTL::ResourceStorageModeShared);
    _mR = _mDevice->newBuffer(bufferLengthVector, MTL::ResourceStorageModeShared);
    _mMatSize = _mDevice->newBuffer(sizeof(uint64_t), MTL::ResourceStorageModeShared);
    _mPartialSums = _mDevice->newBuffer(sizeof(float)*numCols, MTL::ResourceStorageModeShared);
    // data generation
    prepareData();
}
void MetalDeMV::prepareData(){
    // genRandomMatrix(numRows, numCols, 0.7, _mM);
    genRandomMatrix(numCols, 1, 1, _mX);
    for(int i=0;i<numRows;i++){
        genRandomMatrix(numCols, 1, 1, _mMatBuffers[i]);
    }
    uint64_t *matsize = (uint64_t*)_mMatSize->contents();
    matsize[0] = numCols;
}
void MetalDeMV::sendComputeCommand(){
    for(int i=0;i<numRows;i++){
        MTL::CommandBuffer *commandBuffer = _mCommandQueue->commandBuffer();
        assert(commandBuffer!=nullptr);
        MTL::ComputeCommandEncoder *computeEncoder = commandBuffer->computeCommandEncoder();
        computeEncoder->setComputePipelineState(_mComputePSO);
        computeEncoder->setBuffer(_mMatBuffers[i], 0, 0);
        computeEncoder->setBuffer(_mX, 0, 1);
        computeEncoder->setBuffer(_mR, 0, 2);
        computeEncoder->setBuffer(_mMatSize, 0, 3);
        computeEncoder->setBuffer(_mPartialSums, 0, 4);
        int maxRoof = 0;
        if((numCols)<_mComputePSO->maxTotalThreadsPerThreadgroup()){
            maxRoof = numCols;
        } else{
            maxRoof = _mComputePSO->maxTotalThreadsPerThreadgroup();
        }
        computeEncoder->dispatchThreads(MTL::Size::Make(numCols, 1, 1), MTL::Size::Make(maxRoof, 1, 1));
        computeEncoder->endEncoding();
        commandBuffer->commit();
        commandBuffer->waitUntilCompleted();
        // float sum = 0;
        // float *partialSums = (float*)_mPartialSums->contents();
        // for(int j=0;j<numCols;j++){
        //     sum += partialSums[j];
        // }
        // results.push_back(sum);
        // std::cout<<"Made it to this point!"<<std::endl;
    }
    //assert(computeEncoder!=nullptr);
    // verifyResults();
}
void MetalDeMV::verifyResults(){
    float *x = (float*)_mX->contents();
    // float *r = (float*)_mR->contents();
    std::vector<float> res;
    std::vector<float> computed;
    for(int i=0;i<numRows;i++){
        float *m = (float*)_mMatBuffers[i]->contents();
        float sum = 0;
        for(int j=0;j<numCols;j++){
            sum += m[j]*x[j];
        }
        res.push_back(sum);
    }
    
    std::cout<<"Expected:";
    printVector(res.data(), res.size());
    std::cout<<"Actual:";
    printVector(results.data(), results.size());
}
// void MetalDeMV::encodeDeMVCommand(MTL::ComputeCommandEncoder *computeEncoder){
//     computeEncoder->setComputePipelineState(_mComputePSO);
//     computeEncoder->setBuffer(_mM, 0, 0);
//     computeEncoder->setBuffer(_mX, 0, 1);
//     computeEncoder->setBuffer(_mR, 0, 2);
//     computeEncoder->setBuffer(_mMatSize, 0, 3);
//     int maxRoof = 0;
//     std::cout<<"Execution Width:"<<_mComputePSO->threadExecutionWidth()<<std::endl;
//     std::cout<<"Execution Width:"<<_mDevice->
//     if((numRows*numCols)<_mComputePSO->maxTotalThreadsPerThreadgroup()){
//         maxRoof = numRows*numCols;
//     } else{
//         maxRoof = _mComputePSO->maxTotalThreadsPerThreadgroup();
//     }
//     computeEncoder->dispatchThreads(MTL::Size::Make(numRows, numCols, 1), MTL::Size::Make(1, 1, 1));
// }



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
            float gen_dat = (float)rand() / (float)(RAND_MAX);
            data[i] = gen_dat;
            matData[i] = gen_dat;
        } else{
            data[i] = 0;
            matData[i] = 0;
        }
    }
    if(density!=1){
        std::shuffle(&data[0], &data[dataLen - 1], g);
    }
    // std::cout<<"<";
    // for(int i=0;i<dataLen- 1;i++){
    //     std::cout<<data[i]<<",";
    // }
    // std::cout<<data[dataLen - 1]<<">"<<std::endl;
}
