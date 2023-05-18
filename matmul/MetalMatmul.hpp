#include "../common/hpp_single.hpp"

class MetalMatmul{
    public:
    int mat_size;
    uint64_t bufferLength;
    MTL::Device* _mDevice;
    MTL::CommandQueue* _mCommandQueue;
    MTL::ComputePipelineState* _mMatmulFunctionPSO;

    MTL::Buffer* _mM1;
    MTL::Buffer* _mM2;
    MTL::Buffer* _mR;
    MTL::Buffer* _mMatSize;

    MetalMatmul(MTL::Device* device, int mat_size);
    void prepareData();
    void sendComputeCommand();
    void verifyResults();
    
    bool areEqual(float a, float b);

    private:
    void encodeMatmulCommand(MTL::ComputeCommandEncoder* computeEncoder);
    void generateRandomFloatData(MTL::Buffer* buffer);
    ~MetalMatmul();

};