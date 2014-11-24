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
    static IplImage* analyseCoutours(IplImage* img);
    static CvPoint cvtContour2Point(CvSeq *contour);
    static IplImage* flood(IplImage* img, IplImage* im);
    static IplImage* cvtIm2Waterseed(IplImage* img);
    static cv::Mat water(IplImage* BW2, IplImage* imcopy);
};


#endif // ANALYSE_H
