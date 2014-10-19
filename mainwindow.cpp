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
    IplImage* newimage = cvLoadImage(fileName.toLatin1().constData());
    //ui->label->setPixmap(QPixmap::fromImage(Opencv2Qt::imageCvt(newimage)));
   // ui->label->resize(newimage->width,newimage->height);

    cvNamedWindow("111", 0);
    cvShowImage("111",nclseg::seg(newimage));

    cvWaitKey(0);
    cvReleaseImage(&newimage);
    cvDestroyAllWindows();
}
