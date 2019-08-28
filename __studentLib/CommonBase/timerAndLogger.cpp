#define _CRT_SECURE_NO_WARNINGS

#include "timerAndLogger.h"

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

#undef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC 1

CTimerAndLogger& CTimerAndLogger::getInstance(std::string logPath)
{
    static CTimerAndLogger m_logger(logPath);
    return m_logger;
}

void CTimerAndLogger::logTimeWithMsg(std::string msg)
{
    m_logFile << msg << ":;" << (m_endTime - m_startTime) / CLOCKS_PER_SEC << std::endl;
}

CTimerAndLogger::CTimerAndLogger(std::string& logPath) :
    m_startTime(0),
    m_endTime(0),
    m_logFilePath(logPath)
{
    m_logFile.open(logPath, std::ofstream::out | std::ofstream::app);
}

CTimerAndLogger::~CTimerAndLogger()
{
    m_logFile << std::endl;

    m_logFile.close();
}