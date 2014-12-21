#ifndef NCLSEG_H
#define NCLSEG_H

#include <cv.h>
#include <highgui.h>
#include <QDebug>
#include "analyse.h"
#include "feature.h"

class nclseg
{
public:
    nclseg();
public:
    static IplImage* seg(IplImage* img);
    static QStringList seg4train(IplImage* img);
};



#endif // NCLSEG_H
