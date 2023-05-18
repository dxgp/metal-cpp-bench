#include "../common/cpp_single.hpp"
#include <Accelerate/Accelerate.h>
#include "MetalMatmul.hpp"

int main(){
    MTL::Device* device = MTL::CreateSystemDefaultDevice();
    MetalMatmul* met = new MetalMatmul(device, 4);
    
    met->sendComputeCommand();
    
    return 0;
}