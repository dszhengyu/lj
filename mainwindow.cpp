#include "mainwindow.h"
#include "ui_mainwindow.h"

int waitTime = 0;

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

        auto vec = analyse::process(newImage);
        for (auto &elm : vec)
            cvReleaseImage(&elm);

        QPixmap img("nuclei.png");
        ui->label->setScaledContents(true);
        ui->label->setPixmap(img);

        cvReleaseImage(&newImage);
        cvDestroyAllWindows();
    }
}

//选择图片训练SVM
void MainWindow::on_actionTrain_SVM_triggered()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Image"), ".", tr("Image Files(*.png *.jpg *.jpeg *.bmg)"));
    if (fileNames.length() != 0) {
        SvmPredictor svm(fileNames);
        svm.train();
        svm.printMat();
    }
}

//检测SVM训练结果
void MainWindow::on_actionSVM_predict_triggered()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Image"), ".", tr("Image Files(*.png *.jpg *.jpeg *.bmg)"));
    if (fileNames.length() != 0) {
        SvmPredictor svm(fileNames);
        svm.predict();
    }
}

//train ANN
void MainWindow::on_actionTrain_ANN_triggered()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Image"), ".", tr("Image Files(*.png *.jpg *.jpeg *.bmg)"));
    if (fileNames.length() != 0) {
        AnnPredictor ann(fileNames);
        ann.train(false);
        ann.printMat();
    }
}

//ANN predict
void MainWindow::on_actionANN_predict_triggered()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Image"), ".", tr("Image Files(*.png *.jpg *.jpeg *.bmg)"));
    if (fileNames.length() != 0) {
        AnnPredictor ann(fileNames);
        ann.predict();
    }
}


//About
void MainWindow::on_actionAbout_triggered()
{
    QMessageBox aboutMessage(QMessageBox::NoIcon, "关于", "<h2 id=\"software-developed\">Software Developed</h2>\
                             <p>by Appendix </p>\
                             <h2 id=\"algorithm-supported\">Algorithm Supported</h2>\
                             <p>by LJ </p>\
                             <h2 id=\"teacher\">teacher</h2>\
                             <p>liuzhi</p>");
    aboutMessage.setIconPixmap(QPixmap("images/brain.png"));
    aboutMessage.exec();
}


//Gram_Schmidt
void MainWindow::on_actionGram_Schmidt_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"),".",tr("Image Files(*.png *.jpg *.jpeg *.bmg)"));

    if (fileName.length() != 0) {
        //qDebug("%s", fileName.toLocal8Bit().data());
        IplImage* newImage = cvLoadImage(fileName.toLocal8Bit().data());
        gram_schmidt_seg::execGs(newImage);

        QPixmap img("nuclei.jpg");
        ui->label->setScaledContents(true);
        ui->label->setPixmap(img);

        cvReleaseImage(&newImage);
    }
}

void MainWindow::on_action_2_triggered()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Image"), ".", tr("Image Files(*.png *.jpg *.jpeg *.bmg)"));

    if(fileNames.size() != 0) {
        PictureSelection::select(fileNames);
    }
}
