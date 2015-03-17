#ifndef ANNPREDICTOR_H
#define ANNPREDICTOR_H

#include "mainwindow.h"

using std::vector;
using cv::Mat;

class AnnPredictor
{
public:
    explicit AnnPredictor(QStringList &files) :
        fileNames(files),inputs(nullptr), label(nullptr) {};
    void train() const;
    void predict() const;
    void printMat() const;
    ~AnnPredictor() {delete inputs; delete label;};

protected:
    void process() const;
private:
    QStringList fileNames;
    const char *modelName = "annmodel.dat";
    mutable cv::Mat *inputs;
    mutable cv::Mat *label;
};


#endif // ANNPREDICTOR_H
