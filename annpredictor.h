#ifndef ANNPREDICTOR_H
#define ANNPREDICTOR_H

#include "classification.h"

class AnnPredictor : public classification
{
public:
    explicit AnnPredictor(QStringList &files) :
        classification(files),
        bp(nullptr){};
    virtual void train(bool debug = false);
    virtual double predict(bool debug = false);
    ~AnnPredictor() {};
private:
    CvANN_MLP *bp;
};


#endif // ANNPREDICTOR_H
