#pragma once

#include <iostream>
#include <fstream>
#include <assert.h>
#include <string>
#include <vector>
#include <list>

#include <CL/opencl.h>

class __declspec(dllexport) COclAccessing
{
    cl_platform_id m_platfromID;
    cl_device_id m_dev;

    cl_context m_cxGPUContext;
    cl_command_queue m_cqCommandQueue;
    cl_program m_cpProgram;


    //
    // Init, connect part
    ///////////////////////////////

    void getPlatform(std::string deviceName);

    void getDevice();

    void getContext();

    void getCommandQueue();

    char *oclLoadProgSource(const char* cFilename, size_t* szFinalLength);

public:
    
    //
    // base calls
    //////////////////////////////

    const cl_context& getcxGPUContext() { return m_cxGPUContext; };
    const cl_command_queue& getCqCommandQueue() { return m_cqCommandQueue; };
    const cl_program& getCpProgram() { return m_cpProgram; };

    //
    // other calls
    //////////////////////////////

    void listOclDevices();                                                

    void connectToOclDevice(std::string deviceName = "NVIDIA");

    std::string getDeviceAttribute(cl_device_info paramName);             

    void configAndCompileKernel(const char *kernelFileName);

    COclAccessing();
    ~COclAccessing();
};

