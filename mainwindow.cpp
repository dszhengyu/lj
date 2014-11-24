#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Leukocyte Nucleus");
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
//    QMovie *loading = new QMovie("waiting.gif");
//    ui->label_4->setMovie(loading);
//    loading->start();
//    ui->label_4->deleteLater();

    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"),".",tr("Image Files(*.png *.jpg *.jpeg *.bmg)"));
    IplImage* newimage = cvLoadImage(fileName.toLocal8Bit().data());
    //ui->label->setPixmap(QPixmap::fromImage(Opencv2Qt::imageCvt(newimage)));
    //ui->label->resize(newimage->width,newimage->height);
    nclseg::seg(newimage);

    QPixmap img("water3.jpg");
    ui->label->setScaledContents(true);
    ui->label->setPixmap(img);
}

//MyThread::MyThread(QString fileName):
//{
//    this->fileName = fileName;
//}

//void MyThread::loadPic()
//{
//    QMovie *loading = new QMovie("waiting.gif");
//    ui->label_4->setMovie(loading);
//    loading->start();
//}

//void MyThread::showPic()
//{
//    IplImage* newimage = cvLoadImage(fileName.toLocal8Bit().data());
//    nclseg::seg(newimage);
//    QPixmap img("water3.jpg");
//    ui->label->setScaledContents(true);
//    ui->label->setPixmap(img);
//}
