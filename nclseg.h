#ifndef NCLSEG_H
#define NCLSEG_H

#include <cv.h>
#include <highgui.h>

class nclseg
{
public:
    nclseg();
public:
    static IplImage* seg(IplImage* img);
    static int nclseg::Otsu(IplImage* src);
    static IplImage* Bsplict(const IplImage *img);
    static IplImage* Ssplict(const IplImage *img);
    static IplImage* Gsplict(const IplImage *img);
    static void formIE(const IplImage *Is, IplImage *Ig, IplImage *IE);
};
#endif // NCLSEG_H
