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

class SparseMatrix{
    public:
    std::vector<int> row_ptrs;
    std::vector<int> cols;
    std::vector<float> data;
    SparseMatrix(int m, int n, int nnz){
        row_ptrs.push_back(0);
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

