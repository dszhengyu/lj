#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <highgui.h>
#include <cv.h>
#include "opencv2qt.h"
#include <QFileDialog>
#include <QMainWindow>

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

#endif // MAINWINDOW_H
