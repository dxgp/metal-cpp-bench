#include "../common/hpp_single.hpp"

class MetalDot{
    public:
    uint64_t arrayLength = 2048;
    uint64_t bufferSize;
    MTL::Device *_mDevice;
    MTL::ComputePipelineState *_mDotFunctionPSO;
    MTL::CommandQueue * _mCommandQueue;

    MTL::Buffer *_mX;
    MTL::Buffer *_mY;
    MTL::Buffer *_mR;
    MTL::Buffer *_mNumElems;
    MTL::Buffer *_mPartialSums; 

    int _mNumThreadsPerThreadgroup;
    int _mThreadsPerGrid;

    MetalDot(MTL::Device *device, uint64_t arrlen);
    ~MetalDot();
    bool areEqual(float a, float b);
    void prepareData();
    void sendComputeCommand();
    void verifyResults();

    private:
    void encodeDotCommand(MTL::ComputeCommandEncoder *computeEncoder);
    void generateRandomFloatData(MTL::Buffer *buffer);

};