#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "msglib.h"

#include <iostream>
#include <chrono>
#include <cmath>
#include <thread>
#include <iomanip>
#include <string.h>
#include <cassert>
#include "projetIFT630.h"

#define SHOW_INTERVAL_MS 500
#define BLOCK_SIZE 256
#define SHA_PER_ITERATIONS 8'388'608
#define NUMBLOCKS (SHA_PER_ITERATIONS + BLOCK_SIZE - 1) / BLOCK_SIZE

struct messageBuffer { std::string hash; };

static size_t difficulty = 5;

// Output string by the device read by host
char *g_out = nullptr;
unsigned char *g_hash_out = nullptr;
int *g_found = nullptr;

static uint64_t nonce = 0;

__device__ bool checkZeroPadding(unsigned char* sha, size_t difficulty) {

	for (size_t cur_byte = 0; cur_byte < difficulty / 2; ++cur_byte) {
		if (sha[cur_byte] != 0) {
			return false;
		}
	}

	bool isOdd = difficulty % 2 != 0;
	size_t last_byte_check = static_cast<size_t>(difficulty / 2);
	if (isOdd) {
		if (sha[last_byte_check] > 0x0F || sha[last_byte_check] == 0) {
			return false;
		}
	}
	else if (sha[last_byte_check] < 0x0F) return false;

	return true;
}

// Does the same as sprintf(char*, "%d%s", int, const char*) but a bit faster
__device__ size_t nonce_to_str(uint64_t nonce, unsigned char* out) {
	uint64_t result = nonce;
	uint8_t remainder;
	size_t nonce_size = nonce == 0 ? 1 : floor(log10((double)nonce)) + 1;
	size_t i = nonce_size;
	while (result >= 10) {
		remainder = result % 10;
		result /= 10;
		out[--i] = remainder + '0';
	}

	out[0] = result + '0';
	i = nonce_size;
	out[i] = 0;
	return i;
}


extern __shared__ char array[];
__global__ void sha256_kernel(char* out_input_string_nonce, unsigned char* out_found_hash, int *out_found, const char* in_input_string, size_t in_input_string_size, size_t difficulty, uint64_t nonce_offset) 
{

	// If this is the first thread of the block, init the input string in shared memory
	char* in = (char*) &array[0];
	if (threadIdx.x == 0) 
	{
		memcpy(in, in_input_string, in_input_string_size + 1);
	}

	__syncthreads(); // Ensure the input string has been written in SMEM

	uint64_t idx = blockIdx.x * blockDim.x + threadIdx.x;
	uint64_t nonce = idx + nonce_offset;

	// The first byte we can write because there is the input string at the begining
	// Respects the memory padding of 8 bit (char).
	size_t const minArray = static_cast<size_t>(ceil((in_input_string_size + 1) / 8.f) * 8);

	uintptr_t sha_addr = threadIdx.x * (64) + minArray;
	uintptr_t nonce_addr = sha_addr + 32;

	unsigned char* sha = (unsigned char*)&array[sha_addr];
	unsigned char* out = (unsigned char*)&array[nonce_addr];
	memset(out, 0, 32);

	size_t size = nonce_to_str(nonce, out);

	assert(size <= 32);

	SHA256_CTX ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, out, size);
	sha256_update(&ctx, (unsigned char *)in, in_input_string_size);
	sha256_final(&ctx, sha);

	if (checkZeroPadding(sha, difficulty) && atomicExch(out_found, 1) == 0) 
{
		memcpy(out_found_hash, sha, 32);
		memcpy(out_input_string_nonce, out, size);
		memcpy(out_input_string_nonce + size, in, in_input_string_size + 1);
	}
}

void pre_sha256() {
	checkCudaErrors(cudaMemcpyToSymbol(dev_k, host_k, sizeof(host_k), 0, cudaMemcpyHostToDevice));
}

// Prints a 32 bytes sha256 to the hexadecimal form filled with zeroes
void print_hash(const unsigned char* sha256) 
{
	std::cout << "Block mined: ";
	for (size_t i = 0; i < 32; ++i) {
		std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(sha256[i]);
	}
	std::cout << std::dec << std::endl;
}

void print_state() 
{
	if (*g_found) 
	{
		print_hash(g_hash_out);
	}
}

void addBlock(std::string hash) 
{
	nonce = 0;
	const size_t input_size = hash.size();

	// Input string for the device
	char *d_in = nullptr;

	// Create the input string for the device
	cudaMalloc(&d_in, input_size + 1);
	cudaMemcpy(d_in, hash.c_str(), input_size + 1, cudaMemcpyHostToDevice);

	cudaMallocManaged(&g_out, input_size + 32 + 1);
	cudaMallocManaged(&g_hash_out, 32);
	cudaMallocManaged(&g_found, sizeof(int));
	*g_found = 0;

	pre_sha256();

	size_t dynamic_shared_size = (ceil((input_size + 1) / 8.f) * 8) + (64 * BLOCK_SIZE);

	for (;;) {
		sha256_kernel <<<NUMBLOCKS, BLOCK_SIZE, dynamic_shared_size>>> (g_out, g_hash_out, g_found, d_in, input_size, difficulty, nonce);

		cudaError_t err = cudaDeviceSynchronize();
		if (err != cudaSuccess) {
			throw std::runtime_error("Device error");
		}

		nonce += NUMBLOCKS * BLOCK_SIZE;

		print_state();

		if (*g_found) 
		{
			break;
		}
	}

	cudaFree(g_out);
	cudaFree(g_hash_out);
	cudaFree(g_found);

	cudaFree(d_in);

}


int main(int argc, char* argv[]) 
{
	cudaSetDevice(0);
	cudaDeviceSetCacheConfig(cudaFuncCachePreferShared);

	//addBlock depuis argv: HASH DIFFICULTY
	if(argc >= 2)
	{
		std::string h = argv[1];
		difficulty = atoi(argv[2]);
		
		long calcTime;
		calcTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		for(int i =0; i<10; i++)
		{
			std::string newHash(h);
			newHash += i;
			addBlock(newHash);
		}
		std::cout << "Temps pour Cuda: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - calcTime << " ms." << std::endl;
		return 0;
	}

	std::cout << "Bienvenue dans le projet de blockchain!" << std::endl << std::endl;
	std::cout << "Voici les commandes: " << std::endl;
	std::cout << "1- addBlock from to amount (from, to sont des string. amount est un double) -> ajoute un block au blockchain" << std::endl;
	std::cout << "  -> Exemple: addBlock Mathieu Michael 50.00" << std::endl;
	std::cout << "2- cuda -> part une version automatique CUDA de block mining pour le benchmark" << std::endl;
	std::cout << "3- quit" << std::endl;

	while(true)
	{
		std::string s;
		std::cin >> s;

		if(s.find("addBlock") != std::string::npos)
		{
			std::string in;
			std::cin >> in;
			addBlock(in);
		}
		else if(s.find("cuda") != std::string::npos)
		{
			long calcTime;
			calcTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			std::string in = "\nFROM:Mathieu\nTO:Michael\nAMOUNT:50.0";
			for(int i =0; i<10; i++)
			{
				addBlock(in);
			}
			std::cout << "Temps pour Cuda: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - calcTime << " ms." << std::endl;


		}
		else if(s.find("quit") != std::string::npos)
		{
			std::cout << "quitter..." << std::endl;
			break;
		}
	}

	cudaDeviceReset();


	return 0;
}
