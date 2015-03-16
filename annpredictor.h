#ifndef ANNPREDICTOR_H
#define ANNPREDICTOR_H

#include "mainwindow.h"

class AnnPredictor
{
public:
    explicit AnnPredictor(QStringList &fileNames) : files(fileNames) {};
    void train() const;
    void predict() const;

private:
    QStringList files;
    const char *modelName = "annmodel.dat";
    cv::Mat inputs;
    cv::Mat outputs;
    cv::Mat label;
};


#endif // ANNPREDICTOR_H
