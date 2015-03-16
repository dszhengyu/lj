#include "annpredictor.h"

void AnnPredictor::process() const
{
    if (fileNames.length() < 1)
        throw std::runtime_error("No file input!");

    vector<double> labelVec;
    vector<vector<double>> totalFeature;

    for (int i = 0; i < fileNames.length(); i++) {

        //tackle the path name && open the pic
        QString eachFile = fileNames.at(i);//qDebug("%s", eachFile.toLocal8Bit().data());
        IplImage* eachImage = cvLoadImage(eachFile.toLocal8Bit().data());
        cout << eachFile.toLocal8Bit().data() << endl;

        //get the label, first character of the filename
        //but first, extract pure file name from full path name, eg: home/lj/.../1nuclei.jpg"
        QStringList tmp = eachFile.split("/");
        double label = tmp.at(tmp.length() - 1).left(1).toDouble();

        //analyse each pic to get the features and push their correspond label into labelVec;
        auto featureVec = feature::calFeatureVec(eachImage);
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
    label = new Mat(labelVec, true);

    auto iLen = totalFeature.size();
    for (decltype(totalFeature.size()) i = 0; i != iLen; ++i) {
        vector<double> &row = totalFeature.at(i);
        auto jLen = row.size();
        for (decltype(row.size()) j = 0; j != jLen; ++j) {
            (*inputs).at<double>(i, j)= row.at(j);
        }
    }

}

void AnnPredictor::train() const
{
    process();

}

void AnnPredictor::predict() const
{
    if (!inputs) {
        process();
    }


}

