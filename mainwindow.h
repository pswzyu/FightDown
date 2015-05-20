#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

class ScreenManager;
class NuiManager;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QTimer timer;
    ScreenManager* screen;
    NuiManager* nui;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *);

public slots:
    void slt_stopNui();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
