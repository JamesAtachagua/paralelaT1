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

    // 64 = common cache line size
    const int cache_size = 64 / sizeof(float); 

	// sum_k A_ik * B_kj = sum_k A_ik * B^t_jk = C_ij mxl
	std::vector<float> A (m*l, 0); // mxl
	std::vector<float> B (l*n, 0); // lxn
	std::vector<float> Bt(n*l, 0); // nxl
	std::vector<float> C (m*n, 0); // mxn

	auto tstart = std::chrono::system_clock::now();

	//multiplication
	// for(uint64_t i=0;i< m; i++)
	// 	for (uint64_t j = 0; j<n;j++){
	// 		float accum = 0;
	// 		for (uint64_t k = 0; k < l; k++)
	// 			accum += A[i*l+k]*Bt[j*l+k];
	// 		C[i*n+j] = accum;
	// }

    for (int i0 = 0; i0 < m; i0 += cache_size) {
        int imax = i0 + cache_size > m ? m : i0 + cache_size;

        for (int j0 = 0; j0 < n; j0 += cache_size) {
            int jmax = j0 + cache_size > n ? n : j0 + cache_size;

            for (int k0 = 0; k0 < l; k0 += cache_size) {
                int kmax = k0 + cache_size > l ? l : k0 + cache_size;

                for (int j1 = j0; j1 < jmax; ++j1) {
                    int sj = n * j1;

                    for (int i1 = i0; i1 < imax; ++i1) {
                        int mi = n * i1;
                        int ki = l * i1;
                        int kij = ki + j1;

                        for (int k1 = k0; k1 < kmax; ++k1) {
                            C[kij] += A[mi + k1] * B[sj + k1];
                        }
                    }
                }
            }
        }
    }

	auto tend = std::chrono::system_clock::now();

	std::chrono::duration<float,std::milli> duration = tend - tstart;
    std::cout<< "Texec total: " << duration.count() << std::endl;

}