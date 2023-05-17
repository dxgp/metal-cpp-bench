#include "../common/hpp_single.hpp"

class MetalHadamard{
    public:
    int nrows;
    int ncols;
    int bufferLength;
    MTL::Device* _mDevice;
    MTL::ComputePipelineState* _mComputePSO;
    MTL::CommandQueue* _mCommandQueue;
    MTL::Buffer* _mM1; // m1 here
    MTL::Buffer* _mM2; // m2 here
    MTL::Buffer* _mR; // the result matrix

    MetalHadamard(MTL::Device* device, int nrows, int ncols);
    bool areEqual(float a, float b);
    void prepareData();
    void sendComputeCommand();
    void verifyResults();

    private:
    void encodeHadamardCommand(MTL::ComputeCommandEncoder *computeEncoder);
    void generateRandomFloatData(MTL::Buffer* buffer);
};