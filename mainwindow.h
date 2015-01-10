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
#include "nclseg.h"
#include "analyse.h"
#include "classification.h"
#include "opencv2qt.h"
#include "svm.h"
#include "feature.h"
#include "gram_schmidt_seg.h"

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

    void on_actionZoom_In_triggered();

    void on_actionAbout_triggered();

    void on_actionTrain_SVM_triggered();

    void on_actionSVM_predict_triggered();

    void on_actionGram_Schmidt_triggered();

private:
    QImage img;
    QPixmap pic;
    int width, height;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
