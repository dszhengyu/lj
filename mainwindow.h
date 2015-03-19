#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <highgui.h>
#include <cv.h>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <array>
#include <ml.h>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iterator>

using std::cout;
using std::array;
using std::endl;

#include "annpredictor.h"
#include "gram_schmidt_seg.h"
#include "svmpredictor.h"
#include "messagebox.h"

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
    void on_action_triggered();

    void on_actionAbout_triggered();

    void on_actionTrain_SVM_triggered();

    void on_actionSVM_predict_triggered();

    void on_actionGram_Schmidt_triggered();

    void on_actionTrain_ANN_triggered();

    void on_actionANN_predict_triggered();

private:
    QImage img;
    QPixmap pic;
    int width, height;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
