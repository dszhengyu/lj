#include "annpredictor.h"

//extern const int dimension;
//extern const int classNumber;

void AnnPredictor::train(bool debug)
{
    process(debug);
    bp = new CvANN_MLP;
    CvANN_MLP_TrainParams params;
    Mat layerSize = (cv::Mat_<int>(1, 3) << dimension, 2 * dimension, labelCol);
    bp->create(layerSize);
    bp->train(*inputs, *label, Mat());
    bp->save(modelName);
    if (debug)
        cvDestroyAllWindows();
}

double AnnPredictor::predict(bool debug)
{
    process(debug);

    if (!bp) {
        bp = new CvANN_MLP;
        bp->load(modelName);
        if (!bp) {
            QMessageBox aboutMessage(QMessageBox::NoIcon, "出错", "请先训练ANN");
            aboutMessage.setIconPixmap(QPixmap("images/error.png"));
            aboutMessage.exec();
            return -1;
        }
    }

    Mat output;
    bp->predict(*inputs, output);
    printMat(&output);

    return 0;
}
