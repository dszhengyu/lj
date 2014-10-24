#ifndef ANALYSE_H
#define ANALYSE_H

#include <cv.h>
#include <highgui.h>
#include <QDebug>
#include "nclseg.h"

class analyse
{
public:
    analyse();
    static int analyseV(IplImage* img);
    static IplImage* analyseHist(IplImage* channel, int *ptr_max_index); 
    static IplImage* splictB(const IplImage *img);
    static IplImage* splictS(const IplImage *img);
    static IplImage* splictG(const IplImage *img);
    static void formIE(const IplImage *Is, IplImage *Ig, IplImage *IE, int T);
    static IplImage* fillHole(IplImage* hole);
    static int Otsu(IplImage* src);
    static void showImg(IplImage* img, char *s);
    static void  MaxEntropy(IplImage *src,IplImage *dst);
    static IplImage* analyseCoutours(IplImage* img);
    static int analyseCoutours2Circle(IplImage* src, IplImage* dst, struct point *Point);
    static IplImage* analyseCoutours1by1(IplImage* img);
    static int analyseMaxvalue(IplImage* img);
};


#endif // ANALYSE_H
