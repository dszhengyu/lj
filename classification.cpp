#include "classification.h"

void classification::process(bool debug)
{
    if (fileNames.length() < 1)
        throw std::runtime_error("No file input!");

    vector<double> labelVec;
    vector<vector<double>> totalFeature;

    for (int i = 0; i < fileNames.length(); i++) {

        //tackle the path name && open the pic
        QString eachFile = fileNames.at(i);
        IplImage* eachImage = cvLoadImage(eachFile.toLocal8Bit().data());
//        cout << eachFile.toLocal8Bit().data() << endl;

        //get the label, first character of the filename
        //but first, extract pure file name from full path name, eg: home/lj/.../1nuclei.jpg"
        QStringList tmp = eachFile.split("/");
        double label = tmp.at(tmp.length() - 1).left(1).toDouble();

        //analyse each pic to get the features and push their correspond label into labelVec;
        auto featureVec = feature::calFeatureVec(eachImage, debug);
        for (decltype(featureVec.size()) j = 0; j < featureVec.size(); j++) {
            labelVec.push_back(label);
        }
        move(featureVec.begin(), featureVec.end(), std::back_inserter(totalFeature));

        //realese the image cache
        cvReleaseImage(&eachImage);
    }

    //process the Mat used for train and predict
    auto m = labelVec.size();
    inputs = new Mat(m, dimension, CV_64F);
    label = new Mat(m, classNumber, CV_64F);

    for (int i = 0; i < m; ++i) {
        vector<double> &row = totalFeature.at(i);
        for (int j = 0; j < dimension; ++j) {
            inputs->at<double>(i, j) = row.at(j);
        }

        for(int j = 0; j < classNumber; ++j)
            label->at<double>(i, j) = labelVec.at(i);
    }
}

void classification::printMat() const
{
    cout << "inputs Mat!" << endl;
    for (int i = 0; i < inputs->rows; i++) {
        for (int j = 0; j < inputs->cols; j++)
            cout << inputs->at<double>(i, j) << " " ;
        cout << endl;
    }

    cout << endl << "labels Mat" << endl;
    for (int i = 0; i < label->rows; i++)
        cout << label->at<double>(i, 0) << endl;

}
