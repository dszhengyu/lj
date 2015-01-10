#ifndef GRAM_SCHMIDT_SEG_H
#define GRAM_SCHMIDT_SEG_H

#include "mainwindow.h"

class gram_schmidt_seg
{
public:
    gram_schmidt_seg();
    static IplImage *execGs(IplImage *img);
    static CvScalar meanBgr(IplImage *src);
    static CvScalar projection(CvScalar v, CvScalar u);
    static CvScalar calW(CvScalar v1, CvScalar v2, CvScalar v3);
    static IplImage *getFinal(CvScalar w, IplImage *src);
};

#endif // GRAM_SCHMIDT_SEG_H
