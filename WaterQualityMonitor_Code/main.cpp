#include "mainwindow.h"
#include "config.h"
#include <QApplication>
#include <QTranslator>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTextCodec>
#include <QFile>
#include <QDir>
#include <QDebug>
#include "dhintdialog.h"

void initSql()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    db.setHostName("DCJ");
    db.setDatabaseName("DCJ");
    db.setUserName("DCJ");
    db.setPassword("DCJ");

    if(db.open())
    {
        gLogger.log("Database initialized successfully");
    }
    else
    {
        DHintDialog::getInstance(QObject::tr("Init Database Failed"));
        gLogger.log("Failed to open database", DLogger::Log_Warning);
        return;
    }

    QString CREATE_TABLE_CHL1 = "CREATE TABLE IF NOT EXISTS  Chl1_Data(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                                                       "data_R REAL NOT NULL,"
                                                                       "data_T REAL NOT NULL,"
                                                                       "time VARCHAR(20) NOT NULL)";

    QString CREATE_TABLE_CHL2 = "CREATE TABLE IF NOT EXISTS  Chl2_Data(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                                                       "data_R REAL NOT NULL,"
                                                                       "data_T REAL NOT NULL,"
                                                                       "time VARCHAR(20) NOT NULL)";

    QString CREATE_TABLE_CHL3 = "CREATE TABLE IF NOT EXISTS  Chl3_Data(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                                                       "data_R REAL NOT NULL,"
                                                                       "data_T REAL NOT NULL,"
                                                                       "time VARCHAR(20) NOT NULL)";

#if 0
//    qDebug() << QSqlDatabase::drivers();
    QSqlDatabase db= QSqlDatabase::addDatabase("QMYSQL");
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    db.setHostName("192.168.0.81");
    db.setDatabaseName("waterqualitydata");
    db.setUserName("root");
    db.setPassword("sa");
    if(db.open())
    {
        gLogger.log("Database initialized successfully");
    }
    else
    {
        DHintDialog::getInstance(QObject::tr("Init Database Failed"));
        gLogger.log("Failed to open database", DLogger::Log_Warning);
        return;
    }

    QString CREATE_TABLE_CHL1 = "CREATE TABLE IF NOT EXISTS  Chl1_Data(id INTEGER PRIMARY KEY auto_increment,"
                                                                       "data_R REAL NOT NULL,"
                                                                       "data_T REAL NOT NULL,"
                                                                       "time VARCHAR(20) NOT NULL)";

    QString CREATE_TABLE_CHL2 = "CREATE TABLE IF NOT EXISTS  Chl2_Data(id INTEGER PRIMARY KEY auto_increment,"
                                                                       "data_R REAL NOT NULL,"
                                                                       "data_T REAL NOT NULL,"
                                                                       "time VARCHAR(20) NOT NULL)";

    QString CREATE_TABLE_CHL3 = "CREATE TABLE IF NOT EXISTS  Chl3_Data(id INTEGER PRIMARY KEY auto_increment,"
                                                                       "data_R REAL NOT NULL,"
                                                                       "data_T REAL NOT NULL,"
                                                                       "time VARCHAR(20) NOT NULL)";

#endif
    QSqlQuery query;
    bool success = query.exec(CREATE_TABLE_CHL1);
    if(success)
    {
        gLogger.log("Sql:First channel data table created successfully", DLogger::Log_Info);
    }
    else
    {
        gLogger.log("Sql:Failed to create first channel data table", DLogger::Log_Warning);
    }

    success = query.exec(CREATE_TABLE_CHL2);
    if(success)
    {
        gLogger.log("Sql:Second channel data table created successfully", DLogger::Log_Info);
    }
    else
    {
        gLogger.log("Sql:Failed to create second channel data table", DLogger::Log_Warning);
    }

    success = query.exec(CREATE_TABLE_CHL3);
    if(success)
    {
        gLogger.log("Sql:Third channel data table created successfully", DLogger::Log_Info);
    }
    else
    {
        gLogger.log("Sql:Failed to create third channel data table", DLogger::Log_Warning);
    }
}

void loadTheme(QApplication &app)
{
    QFile qssFile(":/qss/app.qss");
    if(!qssFile.open(QFile::ReadOnly))
    {
        gLogger.log("Loading theme failed", DLogger::Log_Error);
    }

    QString strQss = qssFile.readAll();
    qssFile.close();
    app.setStyleSheet(strQss);
    gLogger.log("Successfully loaded theme");
}

bool createLogDir(const QString& fullPath)
{
    QString strDir = QDir::currentPath() + "/" + fullPath;
    qDebug() << strDir;
    QDir dir(strDir);
    if(dir.exists())
    {
        return true;
    }
    else
    {
        bool ok = dir.mkdir(strDir);
        return ok;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationVersion(QStringLiteral("Version 1.0.1 Built on Nov. 1 2019 "));

    MainRetriveConfigParam();
    loadTheme(a);
    createLogDir("Log");

    initSql();

    QTranslator trans;
    if(trans.load(":/language/cn.qm"))
    {
        a.installTranslator(&trans);
        gLogger.log("Successfully loaded language file");
    }
    else
    {
        gLogger.log("Failed to load language file");
    }
    MainWindow w;
    w.setObjectName("MainWindow");
    w.showMaximized();

    return a.exec();
}
