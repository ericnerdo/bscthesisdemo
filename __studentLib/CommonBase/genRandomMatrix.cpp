#define     _CRT_SECURE_NO_WARNINGS
#define     _SCL_SECURE_NO_WARNINGS
#define     D_SCL_SECURE_NO_WARNINGS

#include "genRandomMatrix.h"

#define     SEED_NUMBER     10
#define     MAX_RAND_VAL    255

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>

#include <boost\algorithm\string.hpp>
#include <boost\algorithm\string\split.hpp>

void CGenRandomMatrix::fillRandomValues(std::vector<std::vector<int>>& input, int x, int y)
{
    std::srand(SEED_NUMBER);

    for (size_t i = 0; i < x; i++)
    {
        std::vector<int> tmpVec;
        //tmpVec.reserve(y);
        tmpVec.clear();
        for (size_t j = 0; j < y; j++)
        {
            int randVar = std::rand() % MAX_RAND_VAL;
            tmpVec.push_back(randVar);
        }

        input.push_back(tmpVec);
    }
}


void CGenRandomMatrix::fillRandomValuesFromFile(std::vector<std::vector<int>>& input, const std::string filePath)
{
    std::ifstream fromFile(filePath);

    std::string lineRead;
    std::vector<std::string> lineVector;

    while (std::getline(fromFile, lineRead))
    {
        lineVector.clear();
        boost::algorithm::split(lineVector, lineRead, boost::is_any_of(" "));

        std::vector<int> tmp;
        for (std::string& numTmp : lineVector)
        {
            tmp.push_back(atoi(numTmp.c_str()));
        }
        tmp.pop_back();
        input.push_back(tmp);
    }

    fromFile.close();
}

void CGenRandomMatrix::randomValuesToFile(std::vector<std::vector<int>>& input, std::string filePath)
{
    std::ofstream randomValuesFile;
    randomValuesFile.open(filePath, std::ofstream::out);

    for (std::vector<int>& element : input)
    {
        for (int number : element)
        {
            randomValuesFile << number << " ";
        }

        randomValuesFile << std::endl;
    }

    randomValuesFile << std::endl;

    randomValuesFile.close();
}

CGenRandomMatrix::CGenRandomMatrix()
{
}

CGenRandomMatrix::~CGenRandomMatrix()
{
}
