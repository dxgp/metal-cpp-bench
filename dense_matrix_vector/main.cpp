#include "../common/cpp_single.hpp"
#include "MetalDeMV.hpp"
#include <fstream>
// #include "cblas.h"

#include <Accelerate/Accelerate.h>
typedef std::chrono::microseconds time_unit;
auto unit_name = "microseconds";

auto align_up(int x, int a) { return x + (a - 1) & ~(a - 1); } 
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
bool areEqual(float a, float b) {
    return (fabs(a - b) <= FLT_EPSILON * std::max(1.0f, std::max(a, b)));
}

int main(){
    MTL::Device* device = MTL::CreateSystemDefaultDevice();
    MetalDeMV* demv = new MetalDeMV(device, 512, 512);
    auto start = std::chrono::high_resolution_clock::now();
    demv->sendComputeCommand();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<time_unit>(stop-start).count();
    std::cout<<"Duration: "<<duration<<unit_name<<std::endl;
    int repeats = 100;
    uint64_t sizes[] = {(uint64_t) 128, (uint64_t) 256, (uint64_t) 512, (uint64_t)1024,(uint64_t)2048,(uint64_t)4096, (uint64_t)8192,(uint64_t) 16384, (uint64_t) 32768, (uint64_t) 65536,(uint64_t)262144,(uint64_t)524288};
    auto durations = new float[repeats];
    auto durations_blas = new float[repeats];
    auto durations_vdsp = new float[repeats];
    std::ofstream datafile;
    datafile.open("data.csv");
    datafile << "ArraySize,MetalTime,MetalSD,BLASTime,BLASSD,vDSPTime,vDSPSD\n";

    for(auto size: sizes){
        demv = new MetalDeMV(device, size, size);
        for(size_t repeat = 0; repeat < repeats; repeat++){
            start = std::chrono::high_resolution_clock::now();
            demv->sendComputeCommand();
            stop = std::chrono::high_resolution_clock::now();
            std::cout<<"MADE IT HERE"<<std::endl;
            duration = std::chrono::duration_cast<time_unit>(stop-start).count();
            durations[repeat] = duration;
            
            auto arrayM1 = ((float *) demv->_mM->contents());
            auto arrayX = ((float *) demv->_mX->contents());
            auto arrayR = ((float *) demv->_mR->contents());
            start = std::chrono::high_resolution_clock::now();
            //cblas_sgemv(CblasRowMajor, CblasNoTrans, size, size, 1.0, &arrayM1[0], size, &arrayX[0], 1, 0.0, &arrayR[0], 1);
            cblas_sgemv(CblasRowMajor, CblasNoTrans, size, size, 1.0, &arrayM1[0], size, &arrayX[0], 1, 0.0, &arrayR[0], 1);
            stop = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<time_unit>(stop-start).count();
            durations_blas[repeat] = duration;

            start = std::chrono::high_resolution_clock::now();
            vDSP_mmul(&arrayM1[0], 1, &arrayX[0], 1, &arrayR[0], 1, size, 1, size);
            stop = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<time_unit>(stop-start).count();
            durations_vdsp[repeat] = duration;

        }
        float array_mean;
        float array_std;

        statistics<float>(durations, repeats, array_mean, array_std);
        std::cout<<"******* ARRAY SIZE: "<<size<<"*******"<<std::endl;
        std::cout << "Metal Performance" <<std::endl;
        std::cout << array_mean << unit_name << " \t +/- " << array_std << unit_name << std::endl<< std::endl;
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