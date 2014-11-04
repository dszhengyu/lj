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
};



#endif // NCLSEG_H
