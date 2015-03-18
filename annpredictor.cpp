#include "annpredictor.h"

//extern const int dimension;
//extern const int classNumber;

void AnnPredictor::train(bool debug)
{
    process(debug);
    CvANN_MLP bp;
    CvANN_MLP_TrainParams params;
    Mat layerSize = (cv::Mat_<int>(1, 3) << dimension, 2 * dimension, 6);
    bp.create(layerSize);
    bp.train(*inputs, *label, Mat());
    bp.save(modelName);
    if (debug)
        cvDestroyAllWindows();
}

double AnnPredictor::predict(bool debug)
{
    if (!inputs) {
        process(debug);
    }

    return 0;
}
