#ifndef FEATURE_H
#define FEATURE_H

#include "mainwindow.h"
using std::vector;
using std::move;

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
    static vec2Ddouble calFeatureVec(IplImage* eachImage);
};

const int deminsion = 18;

#endif // FEATURE_H
