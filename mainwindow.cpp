#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"),".",tr("Image Files(*.png *.jpg *.jpeg *.bmg)"));
    IplImage* image = cvLoadImage(fileName.toLatin1().constData());
    IplImage* dst = cvCreateImage(cvGetSize(image),image->depth,1);
    cvCvtColor(image,dst, CV_BGR2GRAY);
    cvNamedWindow("111");
    cvShowImage("111",dst);
}
