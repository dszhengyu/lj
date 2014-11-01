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
    static IplImage* water(IplImage* src, IplImage* dst);
};
struct point {
    CvPoint ciclepoint;
    struct point *next;
};


#endif // NCLSEG_H
