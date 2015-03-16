#ifndef FEATURE_H
#define FEATURE_H

#include "mainwindow.h"

class feature
{
public:
    feature();
    static int getHop(unsigned char i);
    static void formTable(unsigned char *table);
    static QStringList getLBP(IplImage* src);
    static QStringList selectFeature(IplImage* src1, IplImage* src2);
    static QStringList meanRgb(IplImage* src);
    static QStringList getPAR(IplImage* src, int mask);//mask:细胞核1，细胞2，细胞质3
};

const int deminsion = 18;

#endif // FEATURE_H
