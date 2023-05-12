#pragma once

#include "metal-cpp/Foundation/Foundation.hpp"
#include "metal-cpp/Metal/Metal.hpp"
#include <vector>
#include <iostream>


void printVector(std::vector<int> v, std::string vname){
    std::cout<<vname<<": [";
    for(int i=0;i<v.size() - 1;i++){
        std::cout<<v[i]<<",";
    }
    std::cout<<v[v.size() - 1]<<"]"<<std::endl;
}
void printVector(std::vector<float> v, std::string vname){
    std::cout<<vname<<": [";
    for(int i=0;i<v.size() - 1;i++){
        std::cout<<v[i]<<",";
    }
    std::cout<<v[v.size() - 1]<<"]"<<std::endl;
}

class MetalSPMV{
    public:
    MTL::Device * _mDevice;
    MTL::ComputePipelineState * _mSPMVPSO;
    MTL::CommandQueue * _mCommandQueue;

    MTL::Buffer *mat_rp;
    MTL::Buffer *mat_cols;
    MTL::Buffer *mat_data;
    MTL::Buffer *vec_rp;
    MTL::Buffer *vec_cols;
    MTL::Buffer *vec_data;
    MTL::Buffer *result_vector;

    int _mThreadsPerGrid;
    int _mThreadsPerThreadGroup;

    MetalSPMV(MTL::Device *device, uint64_t nnz_matrix, uint64_t nnz_vector, std::vector<int> matdims, int vecdim);
    ~MetalSPMV();

    bool areEqual(float a, float b);
    void prepareData();
    void sendComputeCommand();
    void verifyResults();

    private:
    void encodeSaxpyCommand(MTL::ComputeCommandEncoder * computeEncoder);
    void generateRandomFloatData(MTL::Buffer *buffer);
};



class SparseMatrix{
    public:
    std::vector<int> row_ptrs;
    std::vector<int> cols;
    std::vector<float> data;
    int nrows;
    int ncols;
    SparseMatrix(int nrows, int ncols, float density){
        this->nrows = nrows;
        this->ncols = ncols;
        row_ptrs.push_back(0);
        int nnz = (int) nrows*ncols*density;
        for(int i=0;i<nnz;i++){
            data.push_back((float)rand() / (float)(RAND_MAX)); //gen float between 0 and 1
            cols.push_back(rand()%(ncols)); // random int between 0 and ncols-1 both incl.
        }
        for(int i=1;i<=nrows;i++){
            row_ptrs.push_back(row_ptrs[i-1] + rand()%(ncols+1));
        }
    }
    void printAsDense(){
        std::vector<std::vector<float> > denseMatrix(nrows, std::vector<float>(ncols)); //create empty vector
        for(int i=1;i<row_ptrs.size();i++){
            for(int i=row_ptrs[i-1];i<row_ptrs[i];i++){
                denseMatrix[i][cols[i]] = data[i];
            }
        }
        std::cout<<"[";
        for(int i=0;i<nrows;i++){
            if(i>0){
                std::cout<<" ";
            }
            for(int j=0;j<ncols - 1;j++){
                std::cout<<denseMatrix[i][j]<<",";
            }
            std::cout<<denseMatrix[i][ncols-1]<<std::endl;
        }
    }
    // THIS WAS FOR TESTING. REPLACE WITH RANDOM GENERATION WITH GIVEN PARAMS
    // void createSparseFromDense(std::vector<std::vector<int> > matrix, int m, int n){
    //     for(int i=0;i<m;i++){
    //         int el_count = 0;
    //         for(int j=0;j<n;j++){
    //             if(matrix[i][j]!=0){
    //                 el_count++;
    //                 cols.push_back(j);
    //                 data.push_back(matrix[i][j]);
    //             }
    //         }
    //         row_ptrs.push_back(row_ptrs[i] + el_count);
    //     }
    //     printVector(row_ptrs, "Row pointers");
    //     printVector(cols, "Cols");
    //     printVector(data, "data");
    // }
};

