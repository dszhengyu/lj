#ifndef FEATURE_H
#define FEATURE_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <highgui.h>
#include <cv.h>
#include "nclseg.h"
#include "analyse.h"

class feature
{
public:
    feature();
    static int getHop(unsigned char i);
    static void formTable(unsigned char *table);
    static void LBP(IplImage* src, IplImage* dst);
    static QStringList selectFeature(IplImage* src1, IplImage* src2);
    static QStringList meanRgb(IplImage* src);
    static QStringList getPAR(IplImage* src, int mask);//mask:细胞核1，细胞2，细胞质3
};

#endif // FEATURE_H
