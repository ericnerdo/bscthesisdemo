#include "PatternFindAllOverlap.h"

PatternFinder::PatternFinder() {
	oclDevice.connectToOclDevice(config_usedPlatform);
	oclDevice.configAndCompileKernel(kernelFileName);
	timerOn = true;
}

void PatternFinder::loadFile(const char* fileName = "..\\..\\..\\__studentLib\\inputRandMatrix\\randTest1.txt") {
	std::vector<std::vector<int>> matrix;
	CGenRandomMatrix::fillRandomValuesFromFile(matrix, fileName);
	loadFromVector(matrix);
}

void PatternFinder::loadFromVector(std::vector<std::vector<int>> matrix) {
	rows = matrix.size();
	columns = matrix[0].size();
	kernels = 4 * rows;

	while (rows * columns % kernels != 0)
		kernels--;

	inArray = new int[rows * columns];


	for (size_t i = 0; i < matrix.size(); i++) {
		for (size_t j = 0; j < matrix[i].size(); j++) {
			inArray[i * columns + j] = matrix[i][j];
		}
	}
}


void PatternFinder::setTimerOn(bool timerOn) {
	this->timerOn = timerOn;
}

void PatternFinder::findPattern(std::vector<int> pattern) {
	cl_int ret;
	cl_kernel kernel = NULL;
	CTimerAndLogger& timer = CTimerAndLogger::getInstance("results.txt");

	int bufferSize = rows * columns * sizeof(int);
	int blockSize = rows * columns / kernels;

	std::vector<int> patternNumbers;
	for (size_t i = 0; i < pattern.size(); i++) {
		bool inList = false;
		for (size_t j = 0; j < patternNumbers.size(); j++)
			if (patternNumbers[j] == pattern[i])
				inList = true;
		if (!inList)
			patternNumbers.push_back(pattern[i]);
	}

	int* hash = new int[2 * patternNumbers.size()];
	for (size_t i = 0; i < patternNumbers.size(); i++) {
		hash[2 * i] = patternNumbers[i];
		for (size_t j = pattern.size() - 1; j >= 0; j--)
			if (pattern[j] == patternNumbers[i]) {
				hash[2 * i + 1] = pattern.size() - j;
				break;
			}

	}

	int* patternArray = new int[pattern.size()];
	for (size_t i = 0; i < pattern.size(); i++)
		patternArray[i] = pattern[i];

	int patternLength = pattern.size();
	int numberCount = patternNumbers.size();


	results = new int[rows * columns];
	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < columns; j++)
			results[i * columns + j] = 0;

	if (timerOn) {
		timer.setStartTime();
	}

	/* Memory objects in order of parameter list.*/
	cl_mem memobj_in = clCreateBuffer(oclDevice.getcxGPUContext(), CL_MEM_READ_ONLY, bufferSize, NULL, &ret);
	cl_mem memobj_pattern = clCreateBuffer(oclDevice.getcxGPUContext(), CL_MEM_READ_ONLY, patternLength * sizeof(int), NULL, &ret);
	cl_mem memobj_hash = clCreateBuffer(oclDevice.getcxGPUContext(), CL_MEM_READ_ONLY, 2 * patternNumbers.size() * sizeof(int), NULL, &ret);
	cl_mem memobj_results = clCreateBuffer(oclDevice.getcxGPUContext(), CL_MEM_READ_WRITE, rows * columns * sizeof(int), NULL, &ret);

	ret = clEnqueueWriteBuffer(oclDevice.getCqCommandQueue(), memobj_in, CL_TRUE, 0, bufferSize, inArray, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(oclDevice.getCqCommandQueue(), memobj_pattern, CL_TRUE, 0, patternLength * sizeof(int), patternArray, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(oclDevice.getCqCommandQueue(), memobj_hash, CL_TRUE, 0, 2 * patternNumbers.size() * sizeof(int), hash, 0, NULL, NULL);

	kernel = clCreateKernel(oclDevice.getCpProgram(), "findAllOverlap", &ret);


	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj_in);
	ret = clSetKernelArg(kernel, 1, sizeof(int), (void *)&blockSize);
	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&memobj_pattern);
	ret = clSetKernelArg(kernel, 3, sizeof(int), (void *)&patternLength);
	ret = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&memobj_hash);
	ret = clSetKernelArg(kernel, 5, sizeof(int), (void *)&numberCount);
	ret = clSetKernelArg(kernel, 6, sizeof(cl_mem), (void *)&memobj_results);

	size_t global_item_size = kernels;
	size_t local_item_size = 1;

	ret = clEnqueueNDRangeKernel(oclDevice.getCqCommandQueue(), kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
	ret = clEnqueueReadBuffer(oclDevice.getCqCommandQueue(), memobj_results, CL_TRUE, 0, rows * columns * sizeof(int), results, 0, NULL, NULL);

	if (timerOn) {
		timer.setEndTime();
		timer.logTimeWithMsg("Erik (search for the pattern in randTest1 with 10 * #rows kernels)");
	}


	delete[] hash;
	delete[] patternArray;

	ret = clReleaseKernel(kernel);
	ret = clReleaseMemObject(memobj_in);
	ret = clReleaseMemObject(memobj_pattern);
	ret = clReleaseMemObject(memobj_hash);
	ret = clReleaseMemObject(memobj_results);
}

void PatternFinder::printResults() {
	std::cout << "Pattern matches:\n";
	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < columns; j++) {
			if (results[i * columns + j] == 1) {
				std::cout << i << ". row " << j << ". column\n";
			}
		}
	}
}

int *PatternFinder::getResults() {
	return results;
}

int PatternFinder::getRows() {
	return rows;
}

int PatternFinder::getColumns() {
	return columns;
}

PatternFinder::~PatternFinder() {
	delete[] inArray;
	delete[] results;
}


int main() {
	std::cout << "** Pattern Finder **\n";
	const char* fileName = "..\\..\\..\\__studentLib\\inputRandMatrix\\randTest1.txt";

	std::vector<int> pattern = { 219, 231, 45, 70, 138, 112, 197, 172, 16, 106, 161 };
	PatternFinder patternFinder;

	patternFinder.loadFile(fileName);
	patternFinder.findPattern(pattern);
	patternFinder.printResults();

	system("PAUSE");

	return 0;
}