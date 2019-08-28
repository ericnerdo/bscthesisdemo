#pragma once

#include <vector>
#include <string>

class __declspec(dllexport) CGenRandomMatrix
{

public:

    static void fillRandomValues(std::vector<std::vector<int>>& input, int x = 100, int y = 100);

    static void fillRandomValuesFromFile(std::vector<std::vector<int>>& input,
        const std::string filePath = "..\\..\\Common\\InputRandMatrix\\randMatrixDef.txt");

    static void randomValuesToFile(std::vector<std::vector<int>>& input,
        std::string filePath = "..\\..\\Common\\InputRandMatrix\\randMatrixDef.txt");

private:
    CGenRandomMatrix();
    ~CGenRandomMatrix();
};