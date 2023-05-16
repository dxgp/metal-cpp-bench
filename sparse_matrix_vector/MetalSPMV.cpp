// #pragma once

// #include "metal-cpp/Foundation/Foundation.hpp"
// #include "metal-cpp/Metal/Metal.hpp"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <cassert>
#include <map>

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

template<class T>
static void generateCSR(

    const int   M,
    const int   N,
    const int   num_nonzero_elems,
    const T     val_min,
    const T     val_max,

    int* row_ptrs,
    int* columns,
    T*   values,
    T*   lhs_vector
) {
    auto e = ( repeatable? 0 : chrono::system_clock::now().time_since_epoch().count() )
    uniform_int_distribution  dist_M  { 0,       M - 1   };
    uniform_int_distribution  dist_N  { 0,       N - 1   };
    uniform_real_distribution dist_val{ val_min, val_max };
    vector< std::map<int, T> > nz_cols_per_row;

    for ( int i = 0; i < M; i++ ) {
        nz_cols_per_row.emplace_back();
    }
    for ( int i = 0 ; i < num_nonzero_elems; ) {

        int m   = dist_M( e );
        int n   = dist_N( e );
        T   val = dist_val( e );

        auto& row = nz_cols_per_row[m];
        if ( row.find(n) == row.end() ) {

            row.emplace( make_pair(n, val) );
            i++;
        }
    }

    int sum = 0;
    row_ptrs[0] = 0;
    for ( int i = 0; i < M; i++ ) {

        sum += nz_cols_per_row[i].size();
        row_ptrs[i+1] = sum;
    }

    int pos = 0;
    for ( int i = 0; i < M ; i++ ) {

        auto& row = nz_cols_per_row[i];

        for ( auto it = row.begin(); it != row.end(); it++ ) {

            columns[pos] = it->first;
            values [pos] = it->second;
            pos++;
        }
    }

    for ( int i = 0; i < N; i++ ) {
        lhs_vector[i] = dist_val( e );
    }
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
        generateCSR<float>(nrows,ncols,nnz,0.0f,1.0f,&row_ptrs,&cols,&data);
        
        //createSparseFromDense(nonCSRMatrix, nrows, ncols);
        
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
    void print2DVectorAsMatrix(std::vector<std::vector<float> >  matrix){
        std::cout<<"[";
        for(int i=0;i<matrix.size();i++){
            if(i>0){
                std::cout<<" ";
            }
            for(int j=0;j<matrix[i].size() - 1;j++){
                std::cout<<matrix[i][j]<<",";
            }
            std::cout<<matrix[i][matrix[i].size()-1]<<std::endl;
        }
    }
    
    int generateRandomIntegerBetweenMN(int m, int n) {
        std::random_device rd;  // Obtain a random seed from the hardware
        std::mt19937 gen(rd()); // Initialize the random number generator

        std::uniform_int_distribution<> distribution(m, n); // Define the range [m, n]
        return distribution(gen); // Generate a random integer within the range
    }
    std::vector<std::vector<float> > generateRandomMatrix(int nrows, int ncols, int numElements){
        std::vector<std::vector<float> > matrix(nrows, std::vector<float>(ncols)); //create empty vector
        std::vector<float> matrixElements(nrows*ncols);
        // warming up rand()
        (float)rand() / (float)(RAND_MAX);
        (float)rand() / (float)(RAND_MAX);
        (float)rand() / (float)(RAND_MAX);
        (float)rand() / (float)(RAND_MAX);
        for(int i=0;i<numElements;i++){
            matrixElements[i] = (float)rand() / (float)(RAND_MAX);
        }
        std::random_shuffle(matrixElements.begin(), matrixElements.end());
        for(int i=0;i<nrows;i++){
            for(int j=0;j<ncols;j++){
                matrix[i][j] = matrixElements[i*ncols + j];
            }
        }
        print2DVectorAsMatrix(matrix);
        return matrix;
    }

    // THIS WAS FOR TESTING. REPLACE WITH RANDOM GENERATION WITH GIVEN PARAMS
    void createSparseFromDense(std::vector<std::vector<float> > matrix, int m, int n){
        for(int i=0;i<m;i++){
            int el_count = 0;
            for(int j=0;j<n;j++){
                if(matrix[i][j]!=0){
                    el_count++;
                    cols.push_back(j);
                    data.push_back(matrix[i][j]);
                }
            }
            row_ptrs.push_back(row_ptrs[i] + el_count);
        }
        printVector(row_ptrs, "Row pointers");
        printVector(cols, "Cols");
        printVector(data, "data");
    }
};


int main(){
    srand(100);
    SparseMatrix sm = SparseMatrix(4, 5, 0.5);
    // sm.generateRandomMatrix(4, 5, 10);

    
    return 0;
}
// class MetalSPMV{
//     public:
//     uint64_t arrayLength = 2048
// }