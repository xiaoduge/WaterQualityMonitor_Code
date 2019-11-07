#include "dlogger.h"

std::mutex mt;
DLogger::DLogger(const std::string &dir)
{
    char buf[64];
    time_t now = time(0);
    tm* ltm = localtime(&now);

    sprintf(buf, "%d-%d-%d.log", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday);

    if(dir.empty())
    {
        m_fileName = buf;
    }
    else
    {
        m_fileName = dir + "\\" + buf;
    }

}

void DLogger::log(const char* msg, LogLevel level)
{
    mt.lock();

    FILE* fp = ::fopen(m_fileName.data(), "a+");
    if(!fp)
    {
        mt.unlock();
        return;
    }
    char buf[1024];

    time_t now = time(0);
    tm *ltm = localtime(&now);

    ::sprintf(buf, "%d/%d/%d %d:%d:%d, %s, %d, %s, %d\n",
              1900+ltm->tm_year, ltm->tm_mon, ltm->tm_mday,
              ltm->tm_hour, ltm->tm_min, ltm->tm_sec,
              __FILE__, __LINE__, msg, level);

    ::fwrite(buf, sizeof(char),strlen(buf), fp);
    ::fclose(fp);

    mt.unlock();
}
