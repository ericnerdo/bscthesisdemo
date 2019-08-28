#include <iostream>
#include <string>
#include <vector>
#include <math.h>

#include "CL\opencl.h"

#include <CommonBase\oclAccessing.h>
#include <CommonBase\genRandomMatrix.h>
#include <CommonBase\timerAndLogger.h>

const char *config_usedPlatform = "NVIDIA";
const char *kernelFileName = "./kernel/longestSubSeq2.cl";

int main()
{
	CTimerAndLogger& timer = CTimerAndLogger::getInstance("results.txt");

	COclAccessing oclDevice;
	oclDevice.connectToOclDevice(config_usedPlatform);
	oclDevice.configAndCompileKernel(kernelFileName);

	cl_int ret;
	cl_mem memobj_in = NULL;
	cl_mem memobj_out = NULL;
	//cl_program program = NULL;
	cl_kernel kernel = NULL;


	std::cout << "** Longest Subsequence **\n";


	std::vector<std::vector<int>> matrix;
	CGenRandomMatrix::fillRandomValuesFromFile(matrix, "..\\..\\..\\__studentLib\\inputRandMatrix\\randTest4.txt");

	int rows = matrix.size();
	int columns = matrix[0].size();
	std::cout << columns;
	int kernels = 8;

	while ((rows * columns) % kernels != 0)
		kernels--;

	int blocksize = rows * columns / kernels;

	int seekedNumber = 37;


	int bufferSize = rows * columns * sizeof(int);

	int* inArray = new int[rows * columns];


	for (int i = 0; i < matrix.size(); i++) {
		for (int j = 0; j < matrix[i].size(); j++) {
			inArray[i * columns + j] = matrix[i][j];
		}

	}

	int* outArray = new int[4 * kernels];

	// Create Memory Buffer
	memobj_in = clCreateBuffer(oclDevice.getcxGPUContext(), CL_MEM_READ_ONLY, bufferSize, NULL, &ret);
	memobj_out = clCreateBuffer(oclDevice.getcxGPUContext(), CL_MEM_READ_WRITE, 4 * kernels * sizeof(int), NULL, &ret);

	ret = clEnqueueWriteBuffer(oclDevice.getCqCommandQueue(), memobj_in, CL_TRUE, 0, bufferSize, inArray, 0, NULL, NULL);

	// Create OpenCL Kernel
	kernel = clCreateKernel(oclDevice.getCpProgram(), "longestSubSeq2", &ret);

	// Set OpenCL Kernel Parameters
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj_in);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&memobj_out);
	ret = clSetKernelArg(kernel, 2, sizeof(int), (void *)&blocksize);
	ret = clSetKernelArg(kernel, 3, sizeof(int), (void *)&seekedNumber);




	size_t global_item_size = kernels;
	size_t local_item_size = 1;

	timer.setStartTime();
	// Execute OpenCL Kernel
	ret = clEnqueueNDRangeKernel(oclDevice.getCqCommandQueue(), kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

	ret = clEnqueueReadBuffer(oclDevice.getCqCommandQueue(), memobj_out, CL_TRUE, 0, 4 * kernels * sizeof(int), outArray, 0, NULL, NULL);

	int longest = 0;
	int longestX = 0;
	int longestY = 0;


	for (int i = 0; i < kernels * 4; i += 4) {
		if (outArray[i + 1] > longest) {
			
			longest = outArray[i + 1];
			longestX = i / 4;
			longestY = outArray[i + 2];
		}
	}

	int longestConnected = 0;
	int longestConnectedStartX = 0;
	int longestConnectedStartY = 0;

	int currentStreak = outArray[3];
	int currentStreakY = blocksize - outArray[3];
	int currentStreakKern = 0;

	for (int i = 4; i < (kernels - 1) * 4; i += 4) {
		if (outArray[i] == blocksize) {
			currentStreak += outArray[i];
		 }
		else {
			currentStreak = outArray[i - 1] + outArray[i];
			currentStreakY = blocksize - outArray[i - 1];
			currentStreakKern = i / 4 - 1;
		}

		if (longestConnected < currentStreak) {
			longestConnected = currentStreak;
			longestConnectedStartX = currentStreakKern;
			longestConnectedStartY = currentStreakY;
		}
	}

	if (longestConnected > longest) {
		longest = longestConnected;
		longestX = longestConnectedStartX;
		longestY = longestConnectedStartY;
	}
		
	longestY = (longestX * blocksize + longestY) % columns;
	longestX = (longestX * blocksize) / columns;


	std::cout << "Position of the longest subsequence of " << seekedNumber << ": " << longestX << ',' << longestY << " length: " << longest << '\n';

	delete[] inArray;
	delete[] outArray;

	ret = clReleaseKernel(kernel);
	//ret = clReleaseProgram(program);
	ret = clReleaseMemObject(memobj_in);
	ret = clReleaseMemObject(memobj_out);

	timer.setEndTime();

	timer.logTimeWithMsg("Erik (search for the typed character in 1000x1000)");

	system("PAUSE");

	return 0;
}