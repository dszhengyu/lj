#ifndef NCLSEG_H
#define NCLSEG_H

#include "mainwindow.h"
using std::vector;

class nclseg
{
public:
    nclseg();
public:
    static vector<IplImage *> seg(IplImage* img);
    static QStringList seg4train(IplImage* img);
};



#endif // NCLSEG_H
