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

//打开图片，处理得出细胞核
void MainWindow::on_action_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"),".",tr("Image Files(*.png *.jpg *.jpeg *.bmg)"));

    if (fileName.length() != 0) {
        //qDebug("%s", fileName.toLocal8Bit().data());
        IplImage* newImage = cvLoadImage(fileName.toLocal8Bit().data());
        nclseg::seg(newImage);

        QPixmap img("nuclei.jpg");
        ui->label->setScaledContents(true);
        ui->label->setPixmap(img);
    }
}

//选择图片训练SVM
void MainWindow::on_actionTrain_SVM_triggered()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Image"), ".", tr("Image Files(*.png *.jpg *.jpeg *.bmg)"));
    classification::trainSvm(fileNames);
}

//检测SVM训练结果
void MainWindow::on_actionSVM_predict_triggered()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Image"), ".", tr("Image Files(*.png *.jpg *.jpeg *.bmg)"));
    classification::svmPredict(fileNames);
}

void MainWindow::on_actionZoom_In_triggered()
{
    if (img.isNull())
        ;
    else {

    }
}

void MainWindow::on_actionAbout_triggered()
{
//    QMessageBox::about(NULL, "About", "<h2 id=\"software-developed\">Software Developed</h2>\
//                       <p>by Appendix </p>\
//                       <h2 id=\"algorithm-supported\">Algorithm Supported</h2>\
//                       <p>by LJ </p>\
//                       <h2 id=\"teacher\">teacher</h2>\
//                       <p>liuzhi</p>");
    QMessageBox aboutMessage(QMessageBox::NoIcon, "关于", "<h2 id=\"software-developed\">Software Developed</h2>\
                             <p>by Appendix </p>\
                             <h2 id=\"algorithm-supported\">Algorithm Supported</h2>\
                             <p>by LJ </p>\
                             <h2 id=\"teacher\">teacher</h2>\
                             <p>liuzhi</p>");
    aboutMessage.setIconPixmap(QPixmap("images/brain.png"));
    aboutMessage.exec();
}




