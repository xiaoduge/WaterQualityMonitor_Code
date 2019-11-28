#ifndef CONFIG_H
#define CONFIG_H

#include "dlogger.h"
#include <QMutexLocker>

struct GlobalConfiguration
{
    bool openDataLogger;
};

enum Channel
{
    Channel_first = 0,
    Channel_second,
    Channel_Third,
    Channel_Num
};

extern DLogger gLogger;
extern GlobalConfiguration gConfig;
extern QMutex gSqlMutex;

void MainRetriveConfigParam();
void MainSaveConfigParam();

#endif // CONFIG_H
