// #pragma once

// #include "metal-cpp/Foundation/Foundation.hpp"
// #include "metal-cpp/Metal/Metal.hpp"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <cassert>



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
    int nrows;
    int ncols;
    SparseMatrix(int nrows, int ncols, float density){
        this->nrows = nrows;
        this->ncols = ncols;
        int nnz = (int) nrows*ncols*density;
        int rem_nnz = nnz;
        // warm up rand
        (float)rand() / (float)(RAND_MAX);
        (float)rand() / (float)(RAND_MAX);
        (float)rand() / (float)(RAND_MAX);
        for(int i=0;i<nnz;i++){
            data.push_back((float)rand() / (float)(RAND_MAX)); //gen float between 0 and 1
            cols.push_back(rand()%(ncols)); // random int between 0 and ncols-1 both incl.
        }
        // first try to allocate the number of elements in each row
        for(int i=0;i<nrows;i++){
            int gen_no = (rand()%(rem_nnz + 1)) % (ncols + 1);
            row_ptrs.push_back(gen_no);
            rem_nnz = rem_nnz - (gen_no);
        }
        while(rem_nnz>0){
            for(int i=0;i<row_ptrs.size() - 1;i++){
                if(row_ptrs[i]<ncols && rem_nnz>0){
                    row_ptrs[i]++;
                    rem_nnz--;
                }
            }
        }
        std::cout<<"Rem. NNZ:"<<rem_nnz<<std::endl;
        std::vector<int> row_ptrs_final;
        row_ptrs_final.push_back(0);
        for(int i=1;i<=nrows;i++){
            row_ptrs_final.push_back(row_ptrs_final[i-1] + row_ptrs[i-1]);
        }
        // row_ptrs = row_ptrs_final;
        // while(rem_nnz>0){
        //     cout
        // }
        // int k = nrows - 1;
        // while(rem_nnz!=0){
        //     // int nnz_sub = rand()%(rem_nnz + 1);
        //     // row_ptrs[rand()%(nrows - 1) + 1] = nnz_sub;
        //     // rem_nnz = rem_nnz - nnz_sub;
        //     row_ptrs[k%nrows]++;
        //     rem_nnz--;
        //     k--;
        // }
        printVector(row_ptrs, "row_ptrs");
        printVector(cols, "cols");
        printVector(data, "data");
        printVector(row_ptrs_final, "final_row_ptrs");
    }
    void printAsDense(){
        std::vector<std::vector<float> > denseMatrix(nrows, std::vector<float>(ncols)); //create empty vector
        for(int i=1;i<row_ptrs.size();i++){
            for(int j=row_ptrs[i-1];j<row_ptrs[i];j++){
                denseMatrix[i-1][cols[j]] = data[j];
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

std::vector<int> generateRandomNumber(int min, int max){
    std::vector<int> v;
    for(int i=min;i<=max;i++){
        v.push_back(i);
    }
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(v.begin(), v.end(), g);
    return v;
}
int main(){
    srand(100);
    SparseMatrix sm = SparseMatrix(4, 5, 0.5);
    std::cout<<"Matrix generated"<<std::endl;
    sm.printAsDense();
    
    return 0;
}
// class MetalSPMV{
//     public:
//     uint64_t arrayLength = 2048
// }