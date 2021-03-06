/*all the input pictures' name should not include chinese!!!
 * and the name of picture should be handled,
 * that the first character of name is the label of the nuclei in the picture
 *
 *********************************************************************************
 * the dimension(feature) selected is nuclei's: radius, perimeter, area, roundness, number of parts, cytoplasm's radius, area,
 * and ratio between nuclei and whole
 * (texure????????????how to express????????)
 *
 *
 * 英文无力了，才知道一般特征一开始就要20几个30几个后面特征要有60-70个，跪了，要是英文的话不死翘翘了
 *
 * 1--细胞核面积
 * 2--细胞核周长
 * 3--细胞核圆度（公式待补充）
 * 4--细胞核分成几部分
 * 5--细胞核b分量均值
 * 6--细胞核g分量均值
 * 7--细胞核r分量均值
 *
 * 8--整个细胞的面积
 * 9--周长
 * 10--圆度
 * 11--b
 * 12--g
 * 13--r
 *
 * 14--b
 * 15--g
 * 16--r
 * 17--细胞质面积
 * 18--细胞核与整个细胞面积之比
 *
 * lbp bellow
 * 19--mean
 * 20--energy
 * 21--variance
 *
 *
 *
 *
 * **********************************************************************************
 * the label correspond the type of nuclei
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
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <fstream>
#include <array>
#include <ml.h>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <memory>
#include <string>

#include "feature.h"
#include "messagebox.h"

using std::cout;
using std::array;
using std::endl;
using std::vector;
using std::move;
using std::vector;
using cv::Mat;
using std::string;

class classification
{
public:
    explicit classification(QStringList &files) :
        fileNames(files),inputs(nullptr), label(nullptr),
        featureFileName("X.csv"),
        labelFileName("y.csv") {};
    virtual void train(bool debug = false) = 0;
    virtual double predict(bool debug = false) = 0;
    static void printMat(cv::Mat *m, int rL = -1, int cL = -1);
    void printMat() const;
    ~classification() {delete inputs; delete label;};
    void saveFeatures() const;

protected:
    void process(bool debug = false);
    QStringList fileNames;
    const char *modelName = "model.txt";
    cv::Mat *inputs;
    cv::Mat *label;
    const int labelCol = 1;
private:
    vector<vector<float>> totalFeature;
    vector<float> labelVec;
    const string featureFileName;
    const string labelFileName;
};

#endif // CLASSIFICATION_H
