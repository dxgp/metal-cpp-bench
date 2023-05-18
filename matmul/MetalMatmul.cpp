#include "MetalMatmul.hpp"
void printMTLBuffer(MTL::Buffer *buf, std::string name){
    std::cout<<name<<":"<<"<";
    float *ptr = (float*)buf->contents();
    for(int i=0;i<buf->length()/sizeof(float) - 1;i++){
        std::cout<<ptr[i]<<",";
    }
    std::cout<<ptr[buf->length()/sizeof(float) - 1]<<">";
    std::cout<<std::endl;
}

MetalMatmul::MetalMatmul(MTL::Device* device, int mat_size){
    this->mat_size = mat_size;
    bufferLength = mat_size * mat_size * sizeof(float);
    _mDevice = device;
    NS::Error* error = nullptr;
    MTL::Library* lib = _mDevice->newDefaultLibrary();
    MTL::Function* matmul = lib->newFunction(NS::String::string("matrixMultiply", NS::ASCIIStringEncoding));
    _mMatmulFunctionPSO = _mDevice->newComputePipelineState(matmul, &error);
    _mCommandQueue = _mDevice->newCommandQueue();
    lib->release();

    _mM1 = _mDevice->newBuffer(bufferLength, MTL::ResourceStorageModeShared);
    _mM2 = _mDevice->newBuffer(bufferLength, MTL::ResourceStorageModeShared);   
    _mR = _mDevice->newBuffer(bufferLength, MTL::ResourceStorageModeShared);
    _mMatSize = _mDevice->newBuffer(sizeof(uint), MTL::ResourceStorageModeShared);
    prepareData();
}

void MetalMatmul::sendComputeCommand(){
    std::cout<<"sending compute command"<<std::endl;
    MTL::CommandBuffer* commandBuffer = _mCommandQueue->commandBuffer();
    MTL::ComputeCommandEncoder* enc = commandBuffer->computeCommandEncoder();
    enc->setComputePipelineState(_mMatmulFunctionPSO);
    enc->setBuffer(_mM1, 0, 0);
    enc->setBuffer(_mM2, 0, 1);
    enc->setBuffer(_mR, 0, 2);
    enc->setBuffer(_mMatSize, 0, 3);
    enc->dispatchThreadgroups(MTL::Size::Make(mat_size/2 + 1, mat_size/2 + 1, 1), MTL::Size::Make(2, 2, 1));
    enc->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();

    printMTLBuffer(_mM1, "Mat 1");
    printMTLBuffer(_mM2, "Mat 2");
    printMTLBuffer(_mR, "Result");
}

void MetalMatmul::prepareData(){

    generateRandomFloatData(_mM1);
    generateRandomFloatData(_mM2);
    uint* dat = (uint *)_mMatSize->contents();
    dat[0] = mat_size;
}

void MetalMatmul::generateRandomFloatData(MTL::Buffer* buffer){
    float *dataPtr = (float *)buffer->contents();
    (float)rand() / (float)(RAND_MAX);
    (float)rand() / (float)(RAND_MAX);
    (float)rand() / (float)(RAND_MAX);
    for(uint64_t index = 0; index<buffer->length()/sizeof(float); index++){
        dataPtr[index] = (float)rand() / (float)(RAND_MAX);
    }

}

    