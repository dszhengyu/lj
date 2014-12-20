#ifndef FEATURE_H
#define FEATURE_H

#include <cv.h>
#include <highgui.h>

class feature
{
public:
    feature();
    static int getHop(unsigned char i);
    static void formTable(unsigned char *table);
    static void LBP(IplImage* src, IplImage* dst);
};

#endif // FEATURE_H
