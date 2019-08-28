#include <iostream>
#include <string>
#include <vector>

#include "CL\opencl.h"

#include <CommonBase\oclAccessing.h>
#include <CommonBase\genRandomMatrix.h>
#include <CommonBase\timerAndLogger.h>

const char *config_usedPlatform = "NVIDIA";
const char *kernelFileName = "./kernel/blur.cl";

int main()
{
	COclAccessing oclDevice;
	oclDevice.connectToOclDevice(config_usedPlatform);
	oclDevice.configAndCompileKernel(kernelFileName);

	CTimerAndLogger& timer = CTimerAndLogger::getInstance();


	std::cout << "** Blur **\n";

	timer.setStartTime();

	std::vector<std::vector<int>> matrix;


	CGenRandomMatrix::fillRandomValuesFromFile(matrix, "..\\..\\..\\__studentLib\\inputRandMatrix\\randTest4.txt");

	size_t rows = matrix.size();

	size_t columns = matrix[0].size();
	size_t iterations = 1;




	cl_int ret;
	cl_mem memobj_in = NULL;
	cl_mem memobj_out = NULL;
	//cl_program program = NULL;
	cl_kernel kernel = NULL;

	int bufferSize = rows * columns * sizeof(int);

	int* inArray = new int[rows * columns];
	int* outArray = new int[rows * columns];

	for (size_t i = 0; i < matrix.size(); i++) {
		for (size_t j = 0; j < matrix[i].size(); j++) {
			inArray[i * columns + j] = matrix[i][j];
		}

	}

	memobj_in = clCreateBuffer(oclDevice.getcxGPUContext(), CL_MEM_READ_ONLY, bufferSize, NULL, &ret);
	memobj_out = clCreateBuffer(oclDevice.getcxGPUContext(), CL_MEM_READ_WRITE, bufferSize, NULL, &ret);



	ret = clEnqueueWriteBuffer(oclDevice.getCqCommandQueue(), memobj_in, CL_TRUE, 0, bufferSize, inArray, 0, NULL, NULL);

	kernel = clCreateKernel(oclDevice.getCpProgram(), "blur", &ret);

	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj_in);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&memobj_out);
	ret = clSetKernelArg(kernel, 2, sizeof(int), (void *)&rows);
	ret = clSetKernelArg(kernel, 3, sizeof(int), (void *)&columns);

	size_t global_item_size = rows * columns;
	size_t local_item_size = 1;

	//ret = clEnqueueNDRangeKernel(oclDevice.getCqCommandQueue(), kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

	for (size_t i = 0; i < iterations; i++) {

		// Execute OpenCL Kernel
		ret = clEnqueueNDRangeKernel(oclDevice.getCqCommandQueue(), kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

		if (i != iterations - 1) {
			// Copy data forum output memory buffer to input memory buffer
			clEnqueueCopyBuffer(oclDevice.getCqCommandQueue(), memobj_out, memobj_in, 0, 0, bufferSize, 0, NULL, NULL);
		}
		else {
			// Copy results from the memory buffer
			ret = clEnqueueReadBuffer(oclDevice.getCqCommandQueue(), memobj_out, CL_TRUE, 0, bufferSize, outArray, 0, NULL, NULL);
		}
	}

	//ret = clEnqueueReadBuffer(oclDevice.getCqCommandQueue(), memobj_out, CL_TRUE, 0, bufferSize, outArray, 0, NULL, NULL);

	timer.setEndTime();




	timer.logTimeWithMsg("Erik (blur of randTest4)");

	std::vector<std::vector<int>> out;

	for (size_t i = 0; i < rows; i++) {
		std::vector<int> row;
		for (size_t j = 0; j < columns; j++) {
			row.push_back(outArray[i * columns + j]);
		}
		out.push_back(row);
	}


	CGenRandomMatrix::randomValuesToFile(out, "out.txt");

	delete[] inArray;
	delete[] outArray;

	ret = clReleaseKernel(kernel);
	//ret = clReleaseProgram(program);
	ret = clReleaseMemObject(memobj_in);
	ret = clReleaseMemObject(memobj_out);

	system("PAUSE");

	return 0;
}