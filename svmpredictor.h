#ifndef SVMPREDICTOR_H
#define SVMPREDICTOR_H

#include "classification.h"
#include "svm.h"

class SvmPredictor : public classification
{
public:
    explicit SvmPredictor(QStringList &files) :
        classification(files),
        m(0), dimension(0),
        y(NULL), x(NULL), x_space(NULL),
        model(NULL) {};
    virtual void train(bool debug = false);
    virtual double predict(bool debug = false);
    ~SvmPredictor() {free(y); delete [] x; delete [] x_space; svm_free_and_destroy_model(&model);};
private:
    void castToArray(bool debug = false);
    int m;
    int dimension;
    double *y;
    struct svm_node ** x;
    struct svm_node *x_space;
    struct svm_model *model;
    const char *modelName = "svmModel.txt";
};

#endif // SVMPREDICTOR_H
