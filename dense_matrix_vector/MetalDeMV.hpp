#include "../common/hpp_single.hpp"

class MetalDeMV{
    public:
    uint64_t arrayLength;
    uint64_t bufferLength;
    MTL::Device* _mDevice;
    MTL::ComputePipelineState* _mComputePSO;
    MTL::CommandQueue* _mCommandQueue;
    MTL::Buffer* _mM; // the matrix is here
    MTL::Buffer* _mX; // the vector is here
    MTL::Buffer* _mR; // the result vector
    MTL::Buffer* _mPartialSums;
    std::vector<MTL::Buffer*> _mMatBuffers;
    std::vector<MTL::ComputeCommandEncoder*> _mComputeEncoders;
    std::vector<float> results;
    MTL::Buffer* _mMatSize;
    int numRows;
    int numCols;
    float * matData;
    MetalDeMV(MTL::Device* device, uint64_t nrows, uint64_t ncols);
    bool areEqual(float a, float b);
    void prepareData();
    void sendComputeCommand();
    void genRandomMatrix(uint64_t nrows, uint64_t ncols, float density, MTL::Buffer* buffer);
    void verifyResults();

    private:
    void encodeDeMVCommand(MTL::ComputeCommandEncoder *computeEncoder);
    void generateRandomFloatData(MTL::Buffer *buffer);
    
};