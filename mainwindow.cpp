#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "screenmanager.h"
#include "nuimanager.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    screen = ui->widget_1;

    nui = new NuiManager(screen);
    nui->start(); // 开始nui管理线程

    QObject::connect(&timer, SIGNAL(timeout()), screen, SLOT(gameLoop())); // 将定时器的信号连接
         // 到场景管理器的更新函数

    timer.start(1000 / 30); // 启动定时器
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::slt_stopNui()
{
    nui->should_run = false;
}
void MainWindow::closeEvent(QCloseEvent *e)
{
    slt_stopNui();
    while (!nui->isFinished()){} // 等待nui线程退出
    QMainWindow::closeEvent(e);
}
