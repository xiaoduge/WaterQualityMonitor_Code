#include "mainwindow.h"
#include "config.h"
#include <QApplication>
#include <QTranslator>
#include <QFile>
#include <QDir>
#include <QDebug>

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

    loadTheme(a);
    createLogDir("Log");

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
