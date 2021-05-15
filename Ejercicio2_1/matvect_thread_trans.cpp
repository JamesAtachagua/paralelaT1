// g++ -std=c++17 -g -Wall -o matvect_thread_trans matvect_thread_trans.cpp -pthread
// Este de aqui practicamente son iguales en tiempo
#include <iostream>
#include <cstdint>
#include <vector>
#include <chrono>
#include<pthread.h>

/* To print the matrix */
#define DEBUG         0
/* To set if execute with tranpose or not */
#define TRANS         1
/* To set if execute with block tilling or not */
#define TILLING       0

/*
primer punto => TRANS 1, TILLING 0
segundo punto => TRANS 1, TILLING 1
*/

/* Set the maximun number of threads */
/* 
Para obtener el menor tiempo de ejecución seria usar tantos Thread
como filas existan, sin embargo, el CPU tiene una limitación de #cores
*/
#define MAX_THREAD  8

/*
Set the power of two
Eg. 
    - 2^11 = 2048
    - 2^12 = 4096
*/
#define MAX         11

/* Set the size of matrix */
const uint64_t m=1 <<MAX;
const uint64_t n=1 <<MAX;
const uint64_t l=1 <<MAX;

// sum_k A_ik * B_kj = sum_k A_ik * B^t_jk = C_ij mxl
std::vector<float> A (m*l, 2); // mxl
std::vector<float> B (l, 3); // l
std::vector<float> Bt(n, 0); // n
std::vector<float> C (m, 0); // mxn

/* Variable to know the current core */
int step_i = 0;   // for multiplication operation
int step_i_t = 0; // for transpose operation

void* trans(void* arg)
{
	int core = step_i_t++;

    //transpose
	for (uint64_t k = core * l / MAX_THREAD; k < (core + 1) * l / MAX_THREAD; k++)
		for (uint64_t j = 0; j < n; j++)
			Bt[j] = B[k];
	
	return NULL;
}

void* multi(void* arg)
{
	int core = step_i++;

    //multiplication
	for(uint64_t i = core * m / MAX_THREAD; i < (core + 1) * m / MAX_THREAD; i++) {
		#if TILLING
			float accum = 0;
		#endif
		for (uint64_t k = 0; k < l; k++) {
			#if TRANS
				#if TILLING
					accum += A[i*l + k] * Bt[k]; // with tranpose
				#else
					C[i] += A[i*l + k] * Bt[k]; // with tranpose
				#endif
			#else
				#if TILLING
					accum += A[i*l + k] * B[k]; // without tranpose
				#else
					C[i] += A[i*l + k] * B[k]; // without tranpose
				#endif
			#endif
	    }
		#if TILLING
			C[i] = accum;
		#endif
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