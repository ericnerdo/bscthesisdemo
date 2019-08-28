#include <iostream>
#include <string>
#include <vector>
#include <math.h>

#include "CL\opencl.h"

#include <CommonBase\oclAccessing.h>
#include <CommonBase\genRandomMatrix.h>
#include <CommonBase\timerAndLogger.h>

class PatternFinder
{
public:
	PatternFinder();

	void findPattern(std::vector<int>);

	void loadFile(const char*);

	void loadFromVector(std::vector<std::vector<int>>);

	void setTimerOn(bool);

	void printResults();

	int *getResults();

	int getRows();

	int getColumns();

	~PatternFinder();

private:
	const char *config_usedPlatform = "NVIDIA";
	const char *kernelFileName = "./kernel/patternFinder.cl";

	COclAccessing oclDevice;

	bool timerOn;

	int kernels;
	int rows;
	int columns;

	int *inArray;
	int *results;
};