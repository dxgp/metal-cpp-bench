#include <iostream>
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include "metal-cpp/Foundation/Foundation.hpp"
#include "metal-cpp/Metal/Metal.hpp"
#include "QuartzCore/QuartzCore.hpp"
#include "MetalSaxpy.hpp"
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
    MetalSaxpy *saxpy = new MetalSaxpy(device);
    std::cout<<"Sending compute command"<<std::endl;
    saxpy->sendComputeCommand(1024, saxpy->arrayLength/1024);
    std::cout<<"Compute command done. Beginning verification"<<std::endl;
    saxpy->verifyResults();
    std::cout<<"Verification complete"<<std::endl;
    uint64_t sizes[] = {(uint64_t)1048576, (uint64_t)134217728,(uint64_t) 268435456, (uint64_t) 536870912};
    int repeats =  100;
    auto durations = new float[repeats];
    auto durations_blas = new float[repeats];
    auto durations_vdsp = new float[repeats];
    int NUM_METAL_THREADS_PER_GROUP = 1024;
    
    // -------------------------------------------- METAL TESTING --------------------------------------------
    for(auto size: sizes){
        saxpy->arrayLength = size;
        saxpy->bufferSize = saxpy->arrayLength * sizeof(float);
        for(size_t repeat = 0; repeat<repeats;repeat++){
            // NOTE: CHECK HOW NUM_METAL_THREADS_PER_GROUP VARIES FROM PROCESSOR TO PROCESSOR

            // MEASURING FOR APPLE METAL
            auto start = std::chrono::high_resolution_clock::now();
            // std::cout<<"Sending final compute command"<<std::endl;
            saxpy->sendComputeCommand(NUM_METAL_THREADS_PER_GROUP, saxpy->arrayLength/NUM_METAL_THREADS_PER_GROUP);
            // std::cout<<"Final compute command sent"<<std::endl;
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<time_unit>(stop-start).count();
            // std::cout<<"Adding duration for metal"<<std::endl;
            durations[repeat] = duration;
            // std::cout<<"Duration added. Now getting array from MTL Buffers for BLAS"<<std::endl;
            auto arrayX = ((float *) saxpy->_mX->contents());
            auto arrayY = ((float *) saxpy->_mY->contents());
            auto arrayA = ((float *) saxpy->_ma->contents());
            // std::cout<<"Arrays retrieved. Starting calc for BLAS"<<std::endl;
            // MEASURING FOR BLAS
            start = std::chrono::high_resolution_clock::now();
            // std::cout<<"Starting cblas_saxpy"<<std::endl;
            cblas_saxpy((uint64_t)saxpy->arrayLength, arrayA[0], arrayX, 1, arrayY, 1);
            // std::cout<<"cblas_saxpy finished"<<std::endl;
            stop = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<time_unit>(stop-start).count();
            durations_blas[repeat] = duration;
            // std::cout<<"Calc for BLAS complete. Starting VDSP calc."<<std::endl;
            // MEASURING FOR vDSP
            start = std::chrono::high_resolution_clock::now();
            //vDSP_vsma ( arrayX, 1, &arrayA[0], arrayY, 1, arrayY, 1, saxpy->arrayLength);
            stop = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<time_unit>(stop-start).count();
            durations_vdsp[repeat] = duration;
            // std::cout<<"vDSP calc finished"<<std::endl;
        }
        float array_mean;
        float array_std;
        statistics(durations, repeats, array_mean, array_std);
        std::cout << "Metal Performance" <<std::endl;
        std::cout << array_mean << unit_name << " \t +/- " << array_std << unit_name << std::endl<< std::endl;
        float array_mean_blas;
        float array_std_blas;
        statistics(durations_blas, repeats, array_mean_blas, array_std_blas);
        std::cout << "BLAS Performance" <<std::endl;
        std::cout << array_mean_blas << unit_name << " \t +/- " << array_std_blas << unit_name << std::endl<< std::endl;
        float array_mean_vdsp;
        float array_std_vdsp;
        statistics(durations_vdsp, repeats, array_mean_vdsp, array_std_vdsp);
        std::cout << "BLAS Performance" <<std::endl;
        std::cout << array_mean_vdsp << unit_name << " \t +/- " << array_std_vdsp << unit_name << std::endl<< std::endl;
    }

}