#ifndef PICTURESELECTION_H
#define PICTURESELECTION_H

#include <QStringList>
#include <QString>
#include <highgui.h>
#include <cv.h>
#include <memory>
#include <iostream>
#include <QTextCodec>

#include "analyse.h"

class PictureSelection
{
public:
    PictureSelection();
    static void select(QStringList files);
private:
    static QString path;
};

#endif // PICTURESELECTION_H
