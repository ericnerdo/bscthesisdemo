#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>

#include <CL/opencl.h>

#include "OclAccessing.h"

void COclAccessing::getPlatform(std::string deviceName)
{
    char chBuffer[1024];
    cl_uint num_platforms;
    cl_platform_id* clPlatformIDs;
    cl_int ciErrNum;
    ciErrNum = clGetPlatformIDs(0, NULL, &num_platforms);
    if (ciErrNum == CL_SUCCESS)
    {
        if (num_platforms != 0)
        {
            if ((clPlatformIDs = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id))) != NULL)
            {
                ciErrNum = clGetPlatformIDs(num_platforms, clPlatformIDs, NULL);
                for (cl_uint i = 0; i < num_platforms; ++i)
                {
                    ciErrNum = clGetPlatformInfo(clPlatformIDs[i], CL_PLATFORM_NAME, 1024, &chBuffer, NULL);
                    if (ciErrNum == CL_SUCCESS)
                    {
                        if (strstr(chBuffer, deviceName.c_str()) != NULL)
                        {
                            m_platfromID = clPlatformIDs[i];
                            break;
                        }
                    }
                }

                if (m_platfromID == 0)
                {
                    m_platfromID = clPlatformIDs[0];
                }

                free(clPlatformIDs);
            }
        }
    }
}

void COclAccessing::getDevice()
{
    char cBuffer[1024];
    cl_int ciErrNum;
    cl_uint ciDeviceCount;
    cl_device_id *devices;
    ciErrNum = clGetDeviceIDs(m_platfromID, CL_DEVICE_TYPE_ALL, 0, NULL, &ciDeviceCount);
    if (ciDeviceCount != 0 && ciErrNum == CL_SUCCESS)
    {
        if ((devices = (cl_device_id*)malloc(sizeof(cl_device_id)* ciDeviceCount)) != NULL)
        {
            ciErrNum = clGetDeviceIDs(m_platfromID, CL_DEVICE_TYPE_ALL, ciDeviceCount, devices, &ciDeviceCount);
            if (ciErrNum == CL_SUCCESS)
            {
                //for (cl_uint i = 0; i < ciDeviceCount; ++i)
                //{
                clGetDeviceInfo(devices[0], CL_DEVICE_NAME, sizeof(cBuffer), &cBuffer, NULL);
                //}

                // use the first device on the given platform
                //printf("oclAccessing: use the first device on the given platfrom: %s\n", cBuffer);

                m_dev = devices[0];
                free(devices);
            }
            else
            {
                std::cout << "Error:  clGetDeviceIDs(m_platfromID, CL_DEVICE_TYPE_ALL, ciDeviceCount, devices, &ciDeviceCount)" << std::endl;
            }
        }
    }
}

void COclAccessing::getContext()
{
    cl_int ciErrNum = 0;

    m_cxGPUContext = clCreateContext(0, 1, &m_dev, NULL, NULL, &ciErrNum);

    if (ciErrNum != 0)
        std::cout << "ERROR in GpuAccess, at " + std::to_string(__LINE__) << std::endl;
}

void COclAccessing::getCommandQueue()
{
    cl_int ciErrNum = 0;

    m_cqCommandQueue = clCreateCommandQueue(m_cxGPUContext, m_dev, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE & CL_QUEUE_PROFILING_ENABLE, &ciErrNum);

    if (ciErrNum != 0)
        std::cout << "ERROR in GpuAccess, at " + std::to_string(__LINE__) << std::endl;
}

char *COclAccessing::oclLoadProgSource(const char* cFilename, size_t* szFinalLength)
{
    FILE* pFileStream = NULL;
    size_t szSourceLength;

    if (fopen_s(&pFileStream, cFilename, "rb") != 0)
    {
        return NULL;
    }

    fseek(pFileStream, 0, SEEK_END);
    szSourceLength = ftell(pFileStream);
    fseek(pFileStream, 0, SEEK_SET);

    char* cSourceString = (char *)malloc(szSourceLength + 1);
    if (fread((cSourceString), szSourceLength, 1, pFileStream) != 1)
    {
        fclose(pFileStream);
        free(cSourceString);
        return 0;
    }

    fclose(pFileStream);
    if (szFinalLength != 0)
    {
        *szFinalLength = szSourceLength;
    }

    cSourceString[szSourceLength] = '\0';
    return cSourceString;
}

void COclAccessing::listOclDevices()
{
    int i, j;
    char* value;
    size_t valueSize;
    cl_uint platformCount;
    cl_platform_id* platforms;
    cl_uint deviceCount;
    cl_device_id* devices;
    cl_uint maxComputeUnits;

    // get all platforms
    clGetPlatformIDs(0, NULL, &platformCount);
    platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * platformCount);
    clGetPlatformIDs(platformCount, platforms, NULL);

    for (i = 0; i < platformCount; i++)
    {
        // get all devices
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
        devices = (cl_device_id*)malloc(sizeof(cl_device_id) * deviceCount);
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, NULL);

        // print platform name
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 0, NULL, &valueSize);
        value = (char*)malloc(valueSize);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, valueSize, value, NULL);
        printf("%d. Platform: %s\n\n", i + 1, value);
        free(value);

        // for each device print critical attributes
        for (j = 0; j < deviceCount; j++) {

            // print device name
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 0, NULL, &valueSize);
            value = (char*)malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, valueSize, value, NULL);
            printf("%d. Device: %s\n", j + 1, value);
            free(value);

            // print hardware device version
            clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, 0, NULL, &valueSize);
            value = (char*)malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, valueSize, value, NULL);
            printf(" %d.%d Hardware version: %s\n", j + 1, 1, value);
            free(value);

            // print software driver version
            clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, 0, NULL, &valueSize);
            value = (char*)malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, valueSize, value, NULL);
            printf(" %d.%d Software version: %s\n", j + 1, 2, value);
            free(value);

            // print parallel compute units
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS,
                sizeof(maxComputeUnits), &maxComputeUnits, NULL);
            printf(" %d.%d Parallel compute units: %d\n", j + 1, 4, maxComputeUnits);
        }

        printf("---------------------------\n");

        free(devices);

    }

    free(platforms);
}

void COclAccessing::connectToOclDevice(std::string deviceName)
{
    if (m_cqCommandQueue)
    {
        clReleaseCommandQueue(m_cqCommandQueue);
        m_cqCommandQueue = nullptr;
    }

    if (m_cxGPUContext)
    {
        clReleaseContext(m_cxGPUContext);
        m_cxGPUContext = nullptr;
    }

    if (m_dev)
    {
        //delete m_dev;
        m_dev = nullptr;
    }

    if (m_platfromID)
    {
        //delete m_platfromID;
        m_platfromID = nullptr;
    }

    getPlatform(deviceName);

    getDevice();

    getContext();

    getCommandQueue();
}

std::string COclAccessing::getDeviceAttribute(cl_device_info paramName)
{

    /* TODO: create a map to the device info */


    cl_int err = 0;

    // return value is always string
    std::string deviceInfo;

    // char iteral
    char *value = nullptr;
    size_t valueSize = 0;

    // number returning
    cl_uint numberValue = 0;

    switch (paramName)
    {
    case CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR:
    case CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT:
    case CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT:
    case CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG:
    case CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT:
    case CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE:
    case CL_DEVICE_GLOBAL_MEM_CACHE_SIZE:
    case CL_DEVICE_GLOBAL_MEM_SIZE:
    case CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE:
    case CL_DEVICE_MAX_CONSTANT_ARGS:
    case CL_DEVICE_LOCAL_MEM_TYPE:
    case CL_DEVICE_LOCAL_MEM_SIZE:
    case CL_DEVICE_MAX_COMPUTE_UNITS:
    case CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:
    case CL_DEVICE_MAX_MEM_ALLOC_SIZE:
    case CL_DEVICE_VERSION:
    case CL_DEVICE_TYPE:
    case CL_DEVICE_MAX_PARAMETER_SIZE:
    case CL_DEVICE_MAX_WORK_GROUP_SIZE:
    case CL_DEVICE_MAX_WORK_ITEM_SIZES:
    case CL_DEVICE_GLOBAL_MEM_CACHE_TYPE:
    case CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE:
    {
        err |= clGetDeviceInfo(m_dev, paramName, sizeof(numberValue), &numberValue, NULL);
        //printf("OclParam: %d\n             ", numberValue);
        err == 0 ? deviceInfo = std::to_string(numberValue) : deviceInfo = "ErrorCode " + std::to_string(err);
    }
    break;

    case CL_DEVICE_NAME:
    {
        err = clGetDeviceInfo(m_dev, paramName, 0, NULL, &valueSize);
        value = (char*)malloc(valueSize);
        err |= clGetDeviceInfo(m_dev, paramName, valueSize, value, NULL);
        //printf("OclParam: %s\n             ", value);
        err == 0 ? deviceInfo = value : deviceInfo = "ErrorCode " + std::to_string(err);
        free(value);
    }
    break;

    default:
        deviceInfo = "Not supported";
        break;
    }

    return deviceInfo;
}

void COclAccessing::configAndCompileKernel(const char *kernelFileName)
{
    cl_int ciErrNum = 0;
    size_t szKernelLength = 0;

    // read the source code
    char *cSourceCL = oclLoadProgSource(kernelFileName, &szKernelLength);

    // compile string
    //-cl-fast-relaxed-math -cl-nv-verbose
    std::string str(" -cl-fast-relaxed-math ");

    const char *flags = str.c_str();


    //
    // create the program and compile it
    ////////////////////////////////////////

    if (m_cpProgram != nullptr)
    {
        clReleaseProgram(m_cpProgram);
        m_cpProgram = nullptr;
    }

    m_cpProgram = clCreateProgramWithSource(m_cxGPUContext, 1, (const char **)&cSourceCL, &szKernelLength, &ciErrNum);
    if (ciErrNum != 0)
    {
        char logMsg[255];
        clGetProgramBuildInfo(m_cpProgram, m_dev, CL_PROGRAM_BUILD_LOG, 255, logMsg, NULL);
        printf("%s\n", logMsg);
    }

    ciErrNum = clBuildProgram(m_cpProgram, 1, &m_dev, flags, NULL, NULL);
    if (ciErrNum != 0)
    {
        char logMsg[255];
        clGetProgramBuildInfo(m_cpProgram, m_dev, CL_PROGRAM_BUILD_LOG, 255, logMsg, NULL);
        printf("%s\n", logMsg);
    }
}

COclAccessing::COclAccessing() :
    m_platfromID(nullptr),
    m_dev(nullptr),
    m_cxGPUContext(nullptr),
    m_cqCommandQueue(nullptr),
    m_cpProgram(nullptr)
{
};

COclAccessing::~COclAccessing()
{
    if (m_cpProgram != nullptr)
    {
        clReleaseProgram(m_cpProgram);
        m_cpProgram = nullptr;
    }

    if (m_cqCommandQueue)
    {
        clFinish(m_cqCommandQueue);
        clReleaseCommandQueue(m_cqCommandQueue);
        m_cqCommandQueue = nullptr;
    }

    if (m_cxGPUContext)
    {
        clReleaseContext(m_cxGPUContext);
        m_cxGPUContext = nullptr;
    }

    if (m_dev)
    {
        //delete m_dev;
        m_dev = nullptr;
    }

    if (m_platfromID)
    {
        //delete m_platfromID;
        m_platfromID = nullptr;
    }
}