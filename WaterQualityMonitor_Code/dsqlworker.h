#ifndef DSQLWORKER_H
#define DSQLWORKER_H

#include <QObject>
#include <QMutex>
#include "config.h"

class DSqlWorker : public QObject
{
    Q_OBJECT
public:
    explicit DSqlWorker(QObject *parent = nullptr);
    ~DSqlWorker();

signals:

public slots:
    void updateSqlData(int chl, const float data, const float temp, const QString &strTime);

private:
    QMutex m_mutex;
    static const QString m_insertCommand[Channel_Num];
};



#endif // DSQLWORKER_H
