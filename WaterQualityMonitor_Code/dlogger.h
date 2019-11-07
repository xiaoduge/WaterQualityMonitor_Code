#ifndef DLOGGER_H
#define DLOGGER_H

#include <iostream>
#include <Mutex>
#include <ctime>
#include <string>

extern std::mutex mt;

class Duration
{
public:
    explicit Duration() : m_start(clock()){}

    inline double duration();

private:
    clock_t m_start;
};

double Duration::duration()
{
    return (double)(clock() - m_start) / CLOCKS_PER_SEC;
}

class DLogger
{
public:
    enum LogLevel
    {
        Log_Info = 0,
        Log_Debug,
        Log_Warning,
        Log_Error,
        Log_Fatal,
        Log_Num
    };

public:
    explicit DLogger(const std::string &dir = "");

    void log(const char* msg, LogLevel level = Log_Info);

private:
    std::string m_fileName;
};

#endif // DLOGGER_H
