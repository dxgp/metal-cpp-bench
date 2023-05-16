#include <iostream>
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include "metal-cpp/Foundation/Foundation.hpp"
#include "metal-cpp/Metal/Metal.hpp"
#include "QuartzCore/QuartzCore.hpp"
#include "MetalDeMV.hpp"
#include <fstream>
// #include "cblas.h"

#include <Accelerate/Accelerate.h>

int main(){
    MTL::Device* device = MTL::CreateSystemDefaultDevice();
    MetalDeMV* demv = new MetalDeMV(device, 4, 3);
    demv->sendComputeCommand();
    return 0;
}