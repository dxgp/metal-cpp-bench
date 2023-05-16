#include <iostream>
#include <fstream>

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include "metal-cpp/Foundation/Foundation.hpp"
#include "metal-cpp/Metal/Metal.hpp"
#include "QuartzCore/QuartzCore.hpp"
#include "MetalDot.hpp"
#include "metal-cpp/Metal/MTLCaptureManager.hpp"
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

float Variance(float* samples, int n)
{
     int size = n;

     float variance = 0;
     float t = samples[0];
     for (int i = 1; i < size; i++)
     {
          t += samples[i];
          float diff = ((i + 1) * samples[i]) - t;
          variance += (diff * diff) / ((i + 1.0) *i);
     }

     return variance / (size - 1);
}
float StandardDeviation(float* samples, int n)
{
     return sqrt(Variance(samples, n));
}



auto align_up(int x, int a) { return x + (a - 1) & ~(a - 1); } 

int main(){
    MTL::Device* device = MTL::CreateSystemDefaultDevice();    
    MetalDot *dot = new MetalDot(device, 8192);
    auto start = std::chrono::high_resolution_clock::now();
    dot->sendComputeCommand();
    dot->verifyResults();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<time_unit>(stop-start).count();
    
    uint64_t sizes[] = {(uint64_t) 8192, (uint64_t) 16384, (uint64_t) 65536,(uint64_t)262144,(uint64_t)524288,(uint64_t)1048576, (uint64_t)134217728,(uint64_t) 268435456, (uint64_t) 536870912};
    int repeats =  100;
    auto durations = new float[repeats];
    auto durations_blas = new float[repeats];
    auto durations_vdsp = new float[repeats];
    std::ofstream datafile;
    datafile.open("data.csv");
    datafile << "ArraySize,MetalTime,MetalSD,BLASTime,BLASSD,vDSPTime,vDSPSD\n";
    // -------------------------------------------- METAL TESTING --------------------------------------------
    for(auto size: sizes){
        dot = new MetalDot(device, size);
        auto arrayX = ((float *) dot->_mX->contents());
        auto arrayY = ((float *) dot->_mY->contents());
        for(size_t repeat = 0; repeat<100;repeat++){
            auto start = std::chrono::high_resolution_clock::now();
            dot->sendComputeCommand();
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<time_unit>(stop-start).count();
            durations[repeat] = duration;
        }
        for(size_t repeat = 0; repeat<100;repeat++){
            float blas_result = 0;
            start = std::chrono::high_resolution_clock::now();
            blas_result = cblas_sdot((uint64_t)dot->arrayLength,arrayX, 1, arrayY, 1);
            stop = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<time_unit>(stop-start).count();
            durations_blas[repeat] = duration;
        }
        for(size_t repeat = 0; repeat<100; repeat ++){
            float result = 0;
            start = std::chrono::high_resolution_clock::now();
            vDSP_dotpr( arrayX, 1, arrayY, 1, &result, dot->arrayLength);
            stop = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<time_unit>(stop-start).count();
            durations_vdsp[repeat] = duration;
        }
        float array_mean;
        float array_std;
        statistics<float>(durations, repeats, array_mean, array_std);
        std::cout<<"******* ARRAY SIZE: "<<size<<"*******"<<std::endl;
        std::cout << "Metal Performance" <<std::endl;
        std::cout << array_mean << unit_name << " \t +/- " << array_std << unit_name << " \t +/- " << StandardDeviation(durations, repeats)<<std::endl<< std::endl;
        float array_mean_blas;
        float array_std_blas;
        statistics<float>(durations_blas, repeats, array_mean_blas, array_std_blas);
        std::cout << "BLAS Performance" <<std::endl;
        std::cout << array_mean_blas << unit_name << " \t +/- " << array_std_blas << unit_name << std::endl<< std::endl;
        float array_mean_vdsp;
        float array_std_vdsp;
        statistics<float>(durations_vdsp, repeats, array_mean_vdsp, array_std_vdsp);
        std::cout << "vDSP Performance" <<std::endl;
        std::cout << array_mean_vdsp << unit_name << " \t +/- " << array_std_vdsp << unit_name << std::endl<< std::endl;
        std::string writeString = "";
        writeString += std::to_string(size) + ",";
        writeString += std::to_string(array_mean) + ",";
        writeString += std::to_string(array_std) + ",";
        writeString += std::to_string(array_mean_blas) + ",";
        writeString += std::to_string(array_std_blas) + ",";
        writeString += std::to_string(array_mean_vdsp) + ",";
        writeString += std::to_string(array_std_vdsp) + "\n";
        datafile << writeString;
    }
    datafile.close();
    return 0;
}