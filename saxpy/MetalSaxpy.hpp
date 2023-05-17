#include "../common/hpp_single.hpp"

// unsigned int arrayLength = 60*180*10000; //same as num elements
// unsigned int bufferSize = arrayLength * sizeof(float);


class MetalSaxpy{
    public: 
    uint64_t arrayLength = 60*180*10000; //same as num elements
    uint64_t bufferSize ;
    MTL::Device * _mDevice;
    MTL::ComputePipelineState *_mSaxpyFunctionPSO;
    MTL::CommandQueue * _mCommandQueue;

    MTL::Buffer *_mX;
    MTL::Buffer *_mY;
    MTL::Buffer *_ma;
    MTL::Buffer *_mR;
    MTL::Buffer *_mNumElems;

    
    int _mThreadsPerGrid;
    int _mNumThreadsPerThreadgroup;

    MetalSaxpy(MTL::Device *device, uint64_t arrlen);
    ~MetalSaxpy();

    bool areEqual(float a, float b);
    void prepareData();
    void sendComputeCommand();
    void verifyResults();

    private:
    void encodeSaxpyCommand(MTL::ComputeCommandEncoder *computeEncoder);
    void generateRandomFloatData(MTL::Buffer *buffer);
};

