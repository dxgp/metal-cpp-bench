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
bool areEqual(float a, float b) {
    return (fabs(a - b) <= FLT_EPSILON * std::max(1.0f, std::max(a, b)));
}

auto align_up(int x, int a) { return x + (a - 1) & ~(a - 1); } 

void hadamard_vdsp(float *x, float *y, float *z, int nrows){
    for(int i=0;i<(nrows*nrows);i+=nrows){
        vDSP_vmul(x+i, 1, y+i, 1, z+i, 1, nrows);
    } 
}

void blas_vmul(float *x, float *y, float *z, int nrows){
    cblas_ssbmv(CblasRowMajor, CblasUpper, nrows, 0, 1.0, x, 1, y, 1, 0.0, z, 1);
}

void hadamard_blas(float *x, float *y, float *z, int nrows){
    for(int i=0;i<(nrows*nrows);i+=nrows){
        blas_vmul(x+i, y+i, z+i, nrows);
    } 
}

int main(){
    MTL::Device* device = MTL::CreateSystemDefaultDevice();
    MetalHadamard *prod = new MetalHadamard(device, 10, 10);
    auto start = std::chrono::high_resolution_clock::now();
    prod->sendComputeCommand();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<time_unit>(stop-start).count();

    std::cout<<"Duration: "<<duration<<unit_name<<std::endl;
    uint64_t sizes[] = {(uint64_t) 32, (uint64_t) 64, (uint64_t) 128, (uint64_t) 256, (uint64_t) 512, (uint64_t)1024,(uint64_t)2048,(uint64_t)4096, (uint64_t)8192,(uint64_t) 16384, (uint64_t) 32768, (uint64_t) 65536,(uint64_t)262144,(uint64_t)524288};
    //uint64_t sizes[] = {(uint64_t) 8192, (uint64_t) 16384, (uint64_t) 65536,(uint64_t)262144,(uint64_t)524288,(uint64_t)1048576, (uint64_t)134217728,(uint64_t) 268435456, (uint64_t) 536870912};
    int repeats =  100;
    auto durations = new float[repeats];
    auto durations_blas = new float[repeats];
    auto durations_vdsp = new float[repeats];
    std::ofstream datafile;
    datafile.open("data.csv");
    datafile << "ArraySize,MetalTime,MetalSD,BLASTime,BLASSD,vDSPTime,vDSPSD\n";
    // -------------------------------------------- METAL TESTING --------------------------------------------
    for(auto size: sizes){
        prod = new MetalHadamard(device, size, size);
        for(size_t repeat = 0; repeat<100;repeat++){
            // MEASURING FOR APPLE METAL
            auto start = std::chrono::high_resolution_clock::now();
            prod->sendComputeCommand();
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<time_unit>(stop-start).count();
            durations[repeat] = duration;

            // TRANSFERING DATA TO BE USED BY CPU
            auto arrayM1 = ((float *) prod->_mM1->contents());
            auto arrayM2 = ((float *) prod->_mM2->contents());
            auto arrayR = ((float *) prod->_mR->contents());
            

            // MEASURING FOR vDSP
            start = std::chrono::high_resolution_clock::now();
            //hadamard_vdsp(arrayM1, arrayM2, arrayR, size);
            stop = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<time_unit>(stop-start).count();
            durations_vdsp[repeat] = duration;

            // creating the id-matrix representation of the matrix

            // TESTING BLAS VALUES
            // for(int i=0;i<(size*size);i++){
            //     if(!areEqual(arrayR[i], arrayBlasR[i])){
            //         std::cout<<"ERROR in VDSP computation: "<<arrayR[i]<<" != "<<arrayBlasR[i]<<std::endl;
            //         exit(1);
            //     }
            // }
            // MEASURING FOR vDSP
            // start = std::chrono::high_resolution_clock::now();
            // vDSP_vsma ( arrayX, 1, &arrayA[0], arrayY, 1, arrayY, 1, saxpy->arrayLength);
            // stop = std::chrono::high_resolution_clock::now();
            // duration = std::chrono::duration_cast<time_unit>(stop-start).count();
            // durations_vdsp[repeat] = duration;
        }
        // saxpy->verifyResults();
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
