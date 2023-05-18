#include "../common/cpp_single.hpp"
#include <Accelerate/Accelerate.h>
#include "MetalMatmul.hpp"

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
    MetalMatmul* met = new MetalMatmul(device, 32);
    met->sendComputeCommand();
    met->verifyResults();
    uint64_t sizes[] = {(uint64_t) 32, (uint64_t) 64, (uint64_t) 128,(uint64_t)1024,(uint64_t)2048,(uint64_t)4096, (uint64_t)8192,(uint64_t) 16384};
    int repeats =  100;
    auto durations = new float[repeats];
    auto durations_blas = new float[repeats];
    auto durations_vdsp = new float[repeats];
    std::ofstream datafile;
    datafile.open("data.csv");
    datafile << "ArraySize,MetalTime,MetalSD,BLASTime,BLASSD,vDSPTime,vDSPSD\n";
    for(auto size:sizes){
        met = new MetalMatmul(device, size);
        auto arrayM1 = ((float *) met->_mM1->contents());
        auto arrayM2 = ((float *) met->_mM2->contents());
        auto arrayR = ((float *) met->_mR->contents());
        for(size_t repeat = 0; repeat<2;repeat++){
            auto start = std::chrono::high_resolution_clock::now();
            met->sendComputeCommand();
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<time_unit>(stop-start).count();
            durations[repeat] = duration;
        }
        for(size_t repeat = 0; repeat<100;repeat++){
            auto start = std::chrono::high_resolution_clock::now();
            cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, size, size, size, 1.0, arrayM1, size, arrayM2, size, 0.0, arrayR, size);
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<time_unit>(stop-start).count();
            durations_blas[repeat] = duration;
        }
        for(size_t repeat = 0; repeat<100;repeat++){
            auto start = std::chrono::high_resolution_clock::now();
            vDSP_mmul(arrayM1, 1, arrayM2, 1, arrayR, 1, size, size, size);
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<time_unit>(stop-start).count();
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