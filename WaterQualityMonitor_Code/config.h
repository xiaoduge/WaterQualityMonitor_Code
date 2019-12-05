#ifndef CONFIG_H
#define CONFIG_H

#include "dlogger.h"
#include <QMutexLocker>

struct GlobalConfiguration
{
    bool openDataLogger;
    bool enableTempSlm;
};

struct Factor
{
    float firstFactor;
    float secondFactor;
};

enum Channel
{
    Channel_first = 0,
    Channel_second,
    Channel_Third,
    Channel_Num
};

enum SaveParamType
{
    SaveSqlCfg = 0,
    SaveFactorCfg,
    SaveTempSimCfg
};

extern DLogger gLogger;
extern GlobalConfiguration gConfig;
extern QMutex gSqlMutex;
extern Factor gFactor;
extern float gSimTemp;

void MainRetriveConfigParam();
void MainSaveConfigParam(SaveParamType type);

#endif // CONFIG_H
