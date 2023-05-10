#include <iostream>
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include "metal-cpp/Foundation/Foundation.hpp"
#include "metal-cpp/Metal/Metal.hpp"
#include "QuartzCore/QuartzCore.hpp"
#include "MetalDot.hpp"
// #include "cblas.h"

#include <Accelerate/Accelerate.h>

typedef std::chrono::microseconds time_unit;
auto unit_name = "microseconds";

template <class T>
void statistics(T *array, size_t length, T &array_mean, T &array_std)
{
    array_mean = 0;
    for (size_t repeat = 0; repeat < length; repeat++)
    {
        array_mean += array[repeat];
    }
    array_mean /= length;

    array_std = 0;
    for (size_t repeat = 0; repeat < length; repeat++)
    {
        array_std += pow(array_mean - array[repeat], 2.0);
    }
    array_std /= length;
    array_std = pow(array_std, 0.5);
}

auto align_up(int x, int a) { return x + (a - 1) & ~(a - 1); } 

int main(){
    MTL::Device* device = MTL::CreateSystemDefaultDevice();
    MetalHadamard *prod = new MetalHadamard(device, );

    return 0;
}
