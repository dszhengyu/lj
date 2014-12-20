#include "classification.h"

classification::classification()
{
}

void classification::trainSvm(QStringList fileNames)
{
    for (int i = 0; i < fileNames.length(); i++) {
        QString eachFile = fileNames.at(i);//the path of each picture to train the svm classier
        //qDebug("%s", eachFile.toLocal8Bit().data());

        IplImage* eachImage = cvLoadImage(eachFile.toLocal8Bit().data());
        analyse::showImg(eachImage, "eachImage");
        cvWaitKey(0);
        cvDestroyAllWindows();
        QStringList tmp = eachFile.split("/");
        double lable = tmp.at(tmp.length() - 1).left(1).toDouble();//the label of all nuclei in each picture
        //qDebug("%.2lf", lable);
    }
}
