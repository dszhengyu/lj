#include "opencv2qt.h"

Opencv2Qt::Opencv2Qt()
{
}

QImage Opencv2Qt::imageCvt(const cv::Mat* image)
{
    cv::Mat* imageP;
    if(image->channels()==3)
    {
        imageP = new cv::Mat(*image);
        cv::cvtColor(*imageP,*imageP,CV_BGR2RGB);
        QImage myImage = QImage((const unsigned char*)(imageP->data),imageP->cols,imageP->rows,QImage::Format_RGB888);
        delete imageP;
        return myImage;
    }
    else if(image->channels()==1)
    {
        imageP = new cv::Mat(*image);
        QImage myImage = QImage((const unsigned char*)(imageP->data),imageP->cols,imageP->rows,QImage::Format_Indexed8);
        delete imageP;
        return myImage;
    }
}

QImage Opencv2Qt::imageCvt(const IplImage* image)
{
    return imageCvt(new cv::Mat(image));
}
