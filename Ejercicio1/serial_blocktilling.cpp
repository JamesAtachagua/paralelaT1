// g++ -std=c++17 -o serial_blocktilling serial_blocktilling.cpp
// chmod +x serial_blocktilling
// ./serial_blocktilling
#include <iostream>
#include <cstdint>
#include <vector>
#include <chrono>

int main(){

    /* 2^11 = 2048 and 2^12 = 4096 */
	const uint64_t m=1 <<11;
	const uint64_t n=1 <<11;
	const uint64_t l=1 <<11;

    // Get the size of a block with Cache size and size of float
    const int block_size = 256 / sizeof(float);

	// sum_k A_ik * B_kj = sum_k A_ik * B^t_jk = C_ij mxl
	std::vector<float> A (m*l, 0); // mxl
	std::vector<float> B (l*n, 0); // lxn
	std::vector<float> Bt(n*l, 0); // nxl
	std::vector<float> C (m*n, 0); // mxn

	auto tstart = std::chrono::system_clock::now();

    /* Transpose by blocks */
    /* Iterate by blocks of 256 */
    for (int i = 0; i < n; i += block_size) {
        for (int j = 0; j < n; j += block_size) {
            /* transpose matrix */
            for (int k = i; k < i + block_size; ++k) {
                for (int l = j; l < j + block_size; ++l) {
                    Bt[k + l*n] = B[l + k*n];
                }
            }
        }
    }

	auto tstart_1 = std::chrono::system_clock::now();

    /* Get the current block size */
    for (int i0 = 0; i0 < m; i0 += block_size) {
        int imax = i0 + block_size > m ? m : i0 + block_size;
        /* Get the current block size */
        for (int j0 = 0; j0 < n; j0 += block_size) {
            int jmax = j0 + block_size > n ? n : j0 + block_size;
            /* Get the current block size */
            for (int k0 = 0; k0 < l; k0 += block_size) {
                int kmax = k0 + block_size > l ? l : k0 + block_size;

                /* Real multiplication */
                for (int i = j0; i < jmax; ++i) {
                    for (int j = i0; j < imax; ++j) {
                        float accum = 0;
                        for (int k = k0; k < kmax; ++k) {
                            accum += A[i*l+k] * Bt[j*l+k];
                            // accum += A[n*j+k] * Bt[n*i+k];
                        }
                        C[i*n + j] = accum;
                        // C[l*j + i] = accum;
                    }
                }

            }
        }
    }

    auto tend = std::chrono::system_clock::now();

	std::chrono::duration<float,std::milli> duration_trans = tstart_1 - tstart;
	std::cout<< "Texec Transpose: " << duration_trans.count() << std::endl;

	std::chrono::duration<float,std::milli> duration_mult = tend - tstart_1;
	std::cout<< "Texec Multiplication: " << duration_mult.count() << std::endl;

	std::chrono::duration<float,std::milli> duration = tend - tstart;
    std::cout<< "Texec total: " << duration.count() << std::endl;
}