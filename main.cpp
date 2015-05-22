#include <QApplication>

#include <QTime>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    for (int step = 0; step < 10000; step++)
    {
        qrand(); // 产生一些qrand， 丢弃掉
    }
    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();

    return a.exec();
}
