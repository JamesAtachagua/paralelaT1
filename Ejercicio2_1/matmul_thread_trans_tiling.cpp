// g++ -std=c++17 -g -Wall -o matmul_thread_trans_tiling matmul_thread_trans_tiling.cpp -pthread
// NO FUNCIONA
#include <iostream>
#include <cstdint>
#include <vector>
#include <chrono>
#include<pthread.h>

/* To print the matrix */
#define DEBUG       1
/* To set if execute with tranpose or not */
#define TRANS       0

/* Set the maximun number of threads */
/* 
Para obtener el menor tiempo de ejecución seria usar tantos Thread
como filas existan, sin embargo, el CPU tiene una limitación de #cores
*/
#define MAX_THREAD  4

/*
Set the power of two
Eg. 
    - 2^11 = 2048
    - 2^12 = 4096
*/
#define MAX         2

/* Set the size of matrix */
const uint64_t m=1 <<MAX;
const uint64_t n=1 <<MAX;
const uint64_t l=1 <<MAX;

// sum_k A_ik * B_kj = sum_k A_ik * B^t_jk = C_ij mxl
std::vector<float> A (m*l, 2); // mxl
std::vector<float> B (l*n, 3); // lxn
std::vector<float> Bt(n*l, 0); // nxl
std::vector<float> C (m*n, 0); // mxn

// Get the size of a block with Cache size and size of float
const int block_size = 256 / sizeof(float);

/* Variable to know the current core */
int step_i = 0;   // for multiplication operation
int step_i_t = 0; // for transpose operation

void* trans(void* arg)
{
	int core = step_i_t++;

    //transpose
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
	
	return NULL;
}

void* multi(void* arg)
{
	int core = step_i++;
	// std::cout << "etae" << std::endl;
    //multiplication
	int Row = block_size*core;
	int Col = block_size*core;

	if ((Row < m*block_size) && (Col < l*block_size)) {
		float Cvalue = 0.0;
			for (int i = 0; i < n; ++i) {
				/* A[Row, i] and B[i, Col] */
				std::cout << A[Row*n+i] << " " << B[Col+i*l] << std::endl;
				Cvalue += A[Row*n+i] * B[Col+i*l];
			}
		C[Row*m+Col] = Cvalue;
	}
	
	return NULL;
}

int main(){

	#if DEBUG
		// print matrix A
		for (int x : A)
			std::cout << x << " ";
		std::cout << std::endl;

		// print matrix B
		for (int x : B)
			std::cout << x << " ";
		std::cout << std::endl;
	#endif

	// declaring threads
	pthread_t threads[MAX_THREAD];

	auto tstart = std::chrono::system_clock::now();

	#if TRANS
		// Creating four threads, each evaluating its own part
		for (int i = 0; i < MAX_THREAD; i++) {
			int* p;
			pthread_create(&threads[i], NULL, trans, (void*)(p));
		}

		// joining and waiting for all threads to complete
		for (int i = 0; i < MAX_THREAD; i++)
			pthread_join(threads[i], NULL);	
	#endif

	auto tstart_1 = std::chrono::system_clock::now();

    // Creating four threads, each evaluating its own part
	for (int i = 0; i < MAX_THREAD; i++) {
		int* p;
		pthread_create(&threads[i], NULL, multi, (void*)(p));
	}

	// joining and waiting for all threads to complete
	for (int i = 0; i < MAX_THREAD; i++)
		pthread_join(threads[i], NULL);

	auto tend = std::chrono::system_clock::now();

	#if DEBUG
		// print matrix Bt
		for (int x : C)
		    std::cout << x << " ";
		std::cout << std::endl;
	#endif

	#if TRANS
		std::chrono::duration<float,std::milli> duration_trans = tstart_1 - tstart;
		std::cout<< "Texec Transpose: " << duration_trans.count() << std::endl;
	#endif

	std::chrono::duration<float,std::milli> duration_mult = tend - tstart_1;
	std::cout<< "Texec Multiplication: " << duration_mult.count() << std::endl;

	std::chrono::duration<float,std::milli> duration = tend - tstart;
    std::cout<< "Texec total: " << duration.count() << std::endl;

}