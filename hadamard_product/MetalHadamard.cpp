#include "MetalHadamard.hpp"

MetalHadamard::MetalHadamard(MTL::Device* device, int numRows, int numCols){
    _mDevice = device;
    nrows = numRows;
    ncols = numCols;
    bufferLength = nrows * ncols * sizeof(float);
    NS::Error* error = nullptr;
    MTL::Library* library = _mDevice->newDefaultLibrary();
    MTL::Function* function = library->newFunction(NS::String::string("hadamard_product",NS::ASCIIStringEncoding));
    _mComputePSO = _mDevice->newComputePipelineState(function, &error);
    _mCommandQueue = _mDevice->newCommandQueue();
    _mM1 = _mDevice->newBuffer(bufferLength, MTL::ResourceStorageModeShared);
    _mM2 = _mDevice->newBuffer(bufferLength, MTL::ResourceStorageModeShared);
    _mR = _mDevice->newBuffer(bufferLength, MTL::ResourceStorageModeShared);
    
    prepareData();
}
bool MetalHadamard::areEqual(float a, float b) {
    return (fabs(a - b) <= FLT_EPSILON * std::max(1.0f, std::max(a, b)));
}
void MetalHadamard::verifyResults(){
    float* m1 = (float*)_mM1->contents();
    float* m2 = (float*)_mM2->contents();
    float* r = (float*)_mR->contents();
    for(int i=0;i<nrows*ncols;i++){
        if(!areEqual(m1[i]*m2[i],r[i])){
            std::cout<<"Error at index "<<i<<std::endl;
            std::cout<<"m1["<<i<<"] = "<<m1[i]<<std::endl;
            std::cout<<"m2["<<i<<"] = "<<m2[i]<<std::endl;
            std::cout<<"r["<<i<<"] = "<<r[i]<<std::endl;
            exit(1);
        }
    }
}
void MetalHadamard::encodeHadamardCommand(MTL::ComputeCommandEncoder* encoder){
    encoder->setComputePipelineState(_mComputePSO);
    encoder->setBuffer(_mM1, 0, 0);
    encoder->setBuffer(_mM2, 0, 1);
    encoder->setBuffer(_mR, 0, 2);
    int mttg;
    if(nrows*ncols<_mComputePSO->maxTotalThreadsPerThreadgroup()){
        mttg = nrows*ncols;
    } else{
        mttg = _mComputePSO->maxTotalThreadsPerThreadgroup();
    }
    encoder->dispatchThreads(MTL::Size::Make(nrows*ncols, 1, 1), MTL::Size::Make(mttg, 1, 1));
}

void MetalHadamard::prepareData(){
    generateRandomFloatData(_mM1);
    generateRandomFloatData(_mM2);
}

void MetalHadamard::generateRandomFloatData(MTL::Buffer* buffer){
    float* ptr = (float*)buffer->contents();
    for(int i=0;i<buffer->length()/sizeof(float);i++){
        ptr[i] = (float)rand()/(float)RAND_MAX;
    }
}

void MetalHadamard::sendComputeCommand(){
    MTL::CommandBuffer* commandBuffer = _mCommandQueue->commandBuffer();
    MTL::ComputeCommandEncoder* computeEncoder = commandBuffer->computeCommandEncoder();
    encodeHadamardCommand(computeEncoder);
    computeEncoder->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
}
