#ifndef ANNPREDICTOR_H
#define ANNPREDICTOR_H

#include "mainwindow.h"

using std::vector;
using cv::Mat;

class AnnPredictor
{
public:
    explicit AnnPredictor(QStringList &files) :
        fileNames(files),inputs(nullptr), outputs(nullptr), label(nullptr) {};
    void train() const;
    void predict() const;
    ~AnnPredictor() {delete inputs; delete outputs; delete label;};

protected:
    void process() const;
private:
    QStringList fileNames;
    const char *modelName = "annmodel.dat";
    mutable cv::Mat *inputs;
    mutable cv::Mat *outputs;
    mutable cv::Mat *label;
};

extern const int dimension;

#endif // ANNPREDICTOR_H
