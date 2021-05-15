// g++ -std=c++17 -march=native -O3 -o mat_vect_avx mat_vect_avx.cpp
#include <iostream>
#include <chrono>
#include <immintrin.h> //AVX intrinsics
#include <stdio.h>
using namespace std::chrono;
using namespace std;

#define MAX 8192

int main(){
    double array_a[MAX]= {0};
    double array_b[MAX]= {0};
    double result[MAX]= {0};

    auto start = chrono::steady_clock::now();
    __m256d vector_a, vector_b, vector_result;
    vector_a = _mm256_loadu_pd(array_a);
    vector_b = _mm256_loadu_pd(array_b);

    vector_result = _mm256_add_pd(vector_a,vector_b);
    _mm256_storeu_pd(result,vector_result);
    auto end = chrono::steady_clock::now();

    auto texec = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    std::cout << "Texec: " << texec << std::endl;
  
}