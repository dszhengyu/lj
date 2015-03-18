#ifndef ANALYSE_H
#define ANALYSE_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <highgui.h>
#include <cv.h>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <array>
#include <ml.h>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iterator>

using std::cout;
using std::array;
using std::endl;
using std::vector;

class analyse
{
public:
    analyse();
    static vector<IplImage *> process(IplImage* img, bool debug = false);
    static int analyseV(IplImage* img);
    static IplImage* analyseHist(IplImage* channel, int *ptr_max_index); 
    static IplImage* splictB(const IplImage *img);
    static IplImage* splictS(const IplImage *img);
    static IplImage* splictG(const IplImage *img);
    static void formIE(const IplImage *Is, IplImage *Ig, IplImage *IE, int T);
    static void fillHole(IplImage* hole);
    static int Otsu(IplImage* src);
    static int Otsu2(IplImage* src);
    inline static void showImg(IplImage* img, char *s);
    static IplImage* analyseCoutours(IplImage* img);
    static IplImage* flood(IplImage* img, IplImage* im, int &isalot);
    static IplImage* cvtIm2Waterseed(IplImage* img);
    static cv::Mat water(IplImage* BW2, IplImage* imcopy);
};

inline void analyse::showImg(IplImage *img, char *s)
{
    cvNamedWindow(s, 0);
    cvShowImage(s, img);
}

#endif // ANALYSE_H
