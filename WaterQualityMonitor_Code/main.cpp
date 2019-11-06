#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QFile>

void loadTheme(QApplication &app)
{
    QFile qssFile(":/qss/app.qss");
    qssFile.open(QFile::ReadOnly);
    QString strQss = qssFile.readAll();
    qssFile.close();
    app.setStyleSheet(strQss);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationVersion(QStringLiteral("Version 1.0.1 Built on Nov. 1 2019 "));

    loadTheme(a);

    QTranslator trans;
    if(trans.load(":/language/cn.qm"))
    {
        a.installTranslator(&trans);
    }

    MainWindow w;
    w.setObjectName("MainWindow");
    w.showMaximized();

    return a.exec();
}
