#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_action_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"),".",tr("Image Files(*.png *.jpg *.jpeg *.bmg)"));

    if (fileName.length() != 0) {
        //qDebug("%s", fileName.toLocal8Bit().data());
        IplImage* newImage = cvLoadImage(fileName.toLocal8Bit().data());
        nclseg::seg(newImage);

        QPixmap img("water5.jpg");
        ui->label->setScaledContents(true);
        ui->label->setPixmap(img);
    }
}

void MainWindow::on_actionSVM_triggered()
{
    //QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Image"), ".", tr("Image Files(*.png *.jpg *.jpeg *.bmg)"));
    classification::trainSvm(fileNames);
}
