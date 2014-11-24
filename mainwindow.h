#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <highgui.h>
#include <cv.h>
#include "nclseg.h"
#include "analyse.h"
#include "opencv2qt.h"
#include <QTextCodec>
#include <QMovie>
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();


private:
    Ui::MainWindow *ui;
};

class MyThread
{

public:
    MyThread(QString fileName);
    static void loadPic();
    static void showPic();
    QString fileName;
};

#endif // MAINWINDOW_H
