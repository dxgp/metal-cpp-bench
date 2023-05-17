#include "../common/cpp_single.hpp"
#include "MetalHadamard.hpp"
#include <fstream>
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
    MetalHadamard *prod = new MetalHadamard(device, 10, 10);
    auto start = std::chrono::high_resolution_clock::now();
    prod->sendComputeCommand();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<time_unit>(stop-start).count();
    std::cout<<"Duration: "<<duration<<unit_name<<std::endl;
    return 0;
}
