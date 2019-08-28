#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

class __declspec(dllexport) CTimerAndLogger
{
    std::ofstream m_logFile;
    std::string m_logFilePath;

    time_t m_startTime;
    time_t m_endTime;

public:

    static CTimerAndLogger& getInstance(std::string logPath = "..\\..\\__studentBin\\result.csv");

    void logTimeWithMsg(std::string msg = "");

/// Inline calls

    void setStartTime() { m_startTime = clock(); }

    void setEndTime() { m_endTime = clock(); }

private:

    CTimerAndLogger(std::string& logPath);
    ~CTimerAndLogger();

public:

    CTimerAndLogger(CTimerAndLogger const&) = delete;
    void operator=(CTimerAndLogger const&) = delete;
};
