#ifndef NCLSEG_H
#define NCLSEG_H

#include <cv.h>
#include <highgui.h>
#include <QDebug>
#include "analyse.h"

class nclseg
{
public:
    nclseg();
public:
    static IplImage* seg(IplImage* img);
    static IplImage* watermelon(IplImage* water, IplImage* BW2IE);
    static void cvt32to8(IplImage* src, IplImage* dst);
};
#endif // NCLSEG_H
