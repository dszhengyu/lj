#ifndef OPENCV2QT_H
#define OPENCV2QT_H
#include <QImage>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
class Opencv2Qt
{
public:
    Opencv2Qt();
public:
    static QImage imageCvt(const cv::Mat* image);
    static QImage imageCvt(const IplImage* image);
};

#endif // OPENCV2QT_H
