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
typedef std::chrono::microseconds time_unit;
auto unit_name = "microseconds";

auto align_up(int x, int a) { return x + (a - 1) & ~(a - 1); } 

int main(){
    MTL::Device* device = MTL::CreateSystemDefaultDevice();
    MetalDeMV* demv = new MetalDeMV(device, 1000, 5000000);
    auto start = std::chrono::high_resolution_clock::now();
    demv->sendComputeCommand();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<time_unit>(stop-start).count();
    std::cout<<"Duration: "<<duration<<unit_name<<std::endl;
    float *result = new float[demv->numRows];
    start = std::chrono::high_resolution_clock::now();
    vDSP_mmul(demv->matData, 1, (float *)demv->_mX->contents(), 1, &result[0], 1, demv->numRows, 1, demv->numCols);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<time_unit>(stop-start).count();
    std::cout<<"Duration vDSP: "<<duration<<unit_name<<std::endl;
    return 0;
}