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
bool MetalMatmul::areEqual(float a, float b) {
    return (fabs(a - b) <= FLT_EPSILON * std::max(1.0f, std::max(a, b)));
}
void MetalMatmul::verifyResults(){
    float *m1 = (float *)_mM1->contents();
    float *m2 = (float *)_mM2->contents();
    float *r = (float *)_mR->contents();
    for(int i=0;i<mat_size;i++){
        for(int j=0;j<mat_size;j++){
            float sum = 0;
            for(int k=0;k<mat_size;k++){
                sum += m1[i*mat_size + k] * m2[k*mat_size + j];
            }
            if(!areEqual(sum, r[i*mat_size + j])){
                std::cout<<"Error at "<<i<<","<<j<<std::endl;
                std::cout<<"Expected: "<<sum<<std::endl;
                std::cout<<"Actual: "<<r[i*mat_size + j]<<std::endl;
                return;
            }
        }
    }
    std::cout<<"Results verified"<<std::endl;
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
    MTL::CommandBuffer* commandBuffer = _mCommandQueue->commandBuffer();
    MTL::ComputeCommandEncoder* enc = commandBuffer->computeCommandEncoder();
    enc->setComputePipelineState(_mMatmulFunctionPSO);
    enc->setBuffer(_mM1, 0, 0);
    enc->setBuffer(_mM2, 0, 1);
    enc->setBuffer(_mR, 0, 2);
    enc->setBuffer(_mMatSize, 0, 3);
    enc->dispatchThreadgroups(MTL::Size::Make(mat_size/32 + 1, mat_size/32 + 1, 1), MTL::Size::Make(32, 32, 1));
    enc->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();

    // printMTLBuffer(_mM1, "Mat 1");
    // printMTLBuffer(_mM2, "Mat 2");
    // printMTLBuffer(_mR, "Result");
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

    