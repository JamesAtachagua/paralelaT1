// g++ -std=c++17 -o serial_transpuesto serial_transpuesto.cpp
// chmod +x serial_transpuesto
// ./serial_transpuesto
#include <iostream>
#include <cstdint>
#include <vector>
#include <chrono>

int main(){

    /* 2^11 = 2048 and 2^12 = 4096 */
	const uint64_t m=1 <<11;
	const uint64_t n=1 <<11;
	const uint64_t l=1 <<11;

	// sum_k A_ik * B_kj = sum_k A_ik * B^t_jk = C_ij mxl
	std::vector<float> A (m*l, 0); // mxl
	std::vector<float> B (l*n, 0); // lxn
	std::vector<float> Bt(n*l, 0); // nxl
	std::vector<float> C (m*n, 0); // mxn

	auto tstart = std::chrono::system_clock::now();

	//transpose
	for (uint64_t k = 0; k < l; k++)
		for (uint64_t j = 0; j < n; j++)
			Bt[j*l+k] = B[k*n+j];

	auto tstart_1 = std::chrono::system_clock::now();

	//multiplication
	for(uint64_t i=0;i< m; i++)
		for (uint64_t j = 0; j<n;j++){
			float accum = 0;
			for (uint64_t k = 0; k < l; k++)
				accum += A[i*l+k]*Bt[j*l+k];
			C[i*n+j] = accum;
	}

	auto tend = std::chrono::system_clock::now();

	std::chrono::duration<float,std::milli> duration_trans = tstart_1 - tstart;
	std::cout<< "Texec Transpose: " << duration_trans.count() << std::endl;

	std::chrono::duration<float,std::milli> duration_mult = tend - tstart_1;
	std::cout<< "Texec Multiplication: " << duration_mult.count() << std::endl;

	std::chrono::duration<float,std::milli> duration = tend - tstart;
    std::cout<< "Texec total: " << duration.count() << std::endl;

}