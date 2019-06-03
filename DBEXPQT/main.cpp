#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QDebug>


void UiInit(){
    QFile qss(QDir::currentPath()+"/source/black.QSS");
    qss.open(QFile::ReadOnly);
    qApp->setStyleSheet(qss.readAll());
    qss.close();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //初始化UI
    UiInit();
    w.show();

    return a.exec();
}
