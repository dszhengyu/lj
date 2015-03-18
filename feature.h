#ifndef FEATURE_H
#define FEATURE_H

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
using std::move;

#include "analyse.h"

class feature
{
public:
    typedef vector<vector<double> > vec2Ddouble;
    feature();
    static int getHop(unsigned char i);
    static void formTable(unsigned char *table);
    static vector<double> getLBP(IplImage* src);
    static vector<double> selectFeature(IplImage* src1, IplImage* src2);
    static vector<double> meanRgb(IplImage* src);
    static vector<double> getPAR(IplImage* src, int mask);//mask:细胞核1，细胞2，细胞质3
    static vec2Ddouble calFeatureVec(IplImage* eachImage, bool debug = false);
};


const int dimension = 18;
const int classNumber = 6;

#endif // FEATURE_H
