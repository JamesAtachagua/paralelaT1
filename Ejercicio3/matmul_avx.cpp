//sum_array_avx.cpp
// g++ -std=c++17 -march=native -O3 -o matmul_avx matmul_avx.cpp
#include <iostream>
#include <chrono>
#include <immintrin.h> //AVX intrinsics
#include <stdio.h>
#include <vector>
using namespace std::chrono;
using namespace std;

const uint64_t l=1 <<2;

int nrows = l;
int ncols = l;

int main(){
    float m[nrows*ncols]= {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
    float v[nrows]= {3,3,3,3};
    float result[nrows]= {0,0,0,0};

    auto start = chrono::steady_clock::now();

    // /* Otra forma */
    __m128d *p_m = (__m128d*)m;
    __m128d *p_v = (__m128d*)v;
    __m128d *p_result = (__m128d*)result;

    __m128 acc[l];
    __m128 mat[l];

    for (int row = 0; row < nrows; row += 4) {
        // for (int k = 0; k < l; k++)
        //     acc[k] = _mm_setzero_ps();
        __m128 acc0 = _mm_setzero_ps();
        __m128 acc1 = _mm_setzero_ps();
        __m128 acc2 = _mm_setzero_ps();
        __m128 acc3 = _mm_setzero_ps();
        for (int col = 0; col < ncols; col += 4) {
            __m128 vec = _mm_load_ps(&v[col]);
            // for (int k = 0; k < l; k++)
            //     mat[k] = _mm_load_ps(&m[col + ncols * (row + k)]);
            __m128 mat0 = _mm_load_ps(&m[col + ncols * row]);
            __m128 mat1 = _mm_load_ps(&m[col + ncols * (row + 1)]);
            __m128 mat2 = _mm_load_ps(&m[col + ncols * (row + 2)]);
            __m128 mat3 = _mm_load_ps(&m[col + ncols * (row + 3)]);

            // for (int k = 0; k < l; k++)
            //     acc[k] = _mm_add_ps(acc[k], _mm_mul_ps(mat[k], vec));
            acc0 = _mm_add_ps(acc0, _mm_mul_ps(mat0, vec));
            acc1 = _mm_add_ps(acc1, _mm_mul_ps(mat1, vec));
            acc2 = _mm_add_ps(acc2, _mm_mul_ps(mat2, vec));
            acc3 = _mm_add_ps(acc3, _mm_mul_ps(mat3, vec));
        }
        // acc[0] = _mm_hadd_ps(acc[0], acc[1]);
        // acc[2] = _mm_hadd_ps(acc[2], acc[3]);
        // acc[0] = _mm_hadd_ps(acc[0], acc[1]);
        // _mm_store_ps(&result[row], acc[0]);
        acc0 = _mm_hadd_ps(acc0, acc1);
        acc2 = _mm_hadd_ps(acc2, acc3);
        acc0 = _mm_hadd_ps(acc0, acc2);
        _mm_store_ps(&result[row], acc0);
    }

    auto end = chrono::steady_clock::now();

    auto texec = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    std::cout << "Texec: " << texec << std::endl;
    std::cout << "Result vector: " << result[0] << " " << result[1] << " " << result[2] << " "  << result[3] << " "<< std::endl;

}