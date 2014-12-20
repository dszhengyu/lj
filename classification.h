/*all the input pictures' name should not include chinese!!!
 * and the name of picture should be handled,
 * that the first character of name is the label of the nuclei in the picture
 *
 * 1--
 * 2--
 * 3--
 * 4--
 * 5--
 * 6--
*/


#ifndef CLASSIFICATION_H
#define CLASSIFICATION_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <highgui.h>
#include <cv.h>
#include "nclseg.h"
#include "analyse.h"

class classification
{
public:
    classification();
    static void trainSvm(QStringList fileNames);
};

#endif // CLASSIFICATION_H
