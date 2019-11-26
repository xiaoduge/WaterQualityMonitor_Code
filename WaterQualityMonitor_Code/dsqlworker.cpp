#include "dsqlworker.h"
#include "config.h"
#include "dhintdialog.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMutexLocker>
#include <QMessageBox>
#include <QDebug>

const QString DSqlWorker::m_insertCommand[Channel_Num] =
{
    QStringLiteral("INSERT INTO Chl1_Data(id, data_R, data_T, time) VALUES (:id, :data_R, :data_T, :time)"),
    QStringLiteral("INSERT INTO Chl2_Data(id, data_R, data_T, time) VALUES (:id, :data_R, :data_T, :time)"),
    QStringLiteral("INSERT INTO Chl3_Data(id, data_R, data_T, time) VALUES (:id, :data_R, :data_T, :time)")
};

DSqlWorker::DSqlWorker(QObject *parent) : QObject(parent)
{

}

DSqlWorker::~DSqlWorker()
{

}

void DSqlWorker::updateSqlData(int chl, const float data, const float temp,const QString &strTime)
{
    QMutexLocker mutexLocker(&m_mutex);
    QSqlQuery query;
    query.prepare(m_insertCommand[chl]);
    query.bindValue(":data_R", data);
    query.bindValue(":data_T", temp);
    query.bindValue(":time", strTime);

    bool ret = query.exec();
    if(!ret)
    {
        gLogger.log("Insert to database failed", DLogger::Log_Warning);
    }
}

