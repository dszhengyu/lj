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
    typedef vector<vector<float> > vec2DFloat;
    feature();
    static int getHop(unsigned char i);
    static void formTable(unsigned char *table);
    static vector<float> getLBP(IplImage* src);
    static vector<float> selectFeature(IplImage* src1, IplImage* src2);
    static vector<float> meanRgb(IplImage* src);
    static vector<float> getPAR(IplImage* src, int mask);//mask:细胞核1，细胞2，细胞质3
    static vec2DFloat calFeatureVec(IplImage* eachImage, bool debug = false);
};


const int dimension = 18;
const int classNumber = 6;

#endif // FEATURE_H
