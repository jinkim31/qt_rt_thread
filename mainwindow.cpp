#include "mainwindow.h"
#include "./ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    rtWorker.moveToThread(&thread);
    thread.start();
    QMetaObject::invokeMethod(&rtWorker, &RTWorker::start);

    timer.setInterval(100);
    connect(&timer, &QTimer::timeout, this, &MainWindow::timerCallback);
    connect(&timer, &QTimer::timeout, this, [&]{RTThreadWorker::callQueued(&rtWorker, &RTWorker::startRTSlot, 100);});
    timer.start();
}

MainWindow::~MainWindow()
{
    delete ui;

    rtWorker.stop();
    thread.quit();
    thread.wait();
}

void MainWindow::timerCallback()
{

}
