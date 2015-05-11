#include "classification.h"

void classification::process(bool debug)
{
    if (fileNames.length() < 1)
        throw std::runtime_error("No file input!");

    for (int i = 0; i < fileNames.length(); i++) {

        //tackle the path name && open the pic
        QString eachFile = fileNames.at(i);
        IplImage* eachImage = cvLoadImage(eachFile.toLocal8Bit().data());
//        cout << eachFile.toLocal8Bit().data() << endl;

        //get the label, first character of the filename
        //but first, extract pure file name from full path name, eg: home/lj/.../1nuclei.jpg"
        QStringList tmp = eachFile.split("/");
        float label = tmp.at(tmp.length() - 1).left(1).toDouble();

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
    int m = static_cast<int>(labelVec.size());
    inputs = new Mat(m, dimension, CV_32FC1);
    label = new Mat(m, labelCol, CV_32FC1);

    for (int i = 0; i < m; ++i) {
        vector<float> &row = totalFeature.at(i);
        for (int j = 0; j < dimension; ++j) {
            inputs->at<float>(i, j) = row.at(j);
        }

        for(int j = 0; j < labelCol; ++j)
            label->at<float>(i, j) = labelVec.at(i);
    }

    //save features
    saveFeatures();
}

void classification::printMat(cv::Mat *m, int rL, int cL)
{
    rL = rL > 0 ? rL : m->rows;
    cL = cL > 0 ? cL : m->cols;
    for (int i = 0; i < rL; i++) {
        for (int j = 0; j < cL; j++)
            cout << m->at<float>(i, j) << " " ;
        cout << endl;
    }
}

void classification::printMat() const
{
    cout << "inputs Mat!" << endl;
    printMat(inputs);

    cout << endl << "labels Mat" << endl;
    printMat(label, -1, 1);

}
void classification::saveFeatures() const
{
    std::ofstream featureFile(featureFileName);
    for(auto &row : totalFeature) {
        for (auto &perItem : row)
            featureFile << perItem << ',';
        featureFile << '\n';
    }
    featureFile.close();

    std::ofstream labelFile(labelFileName);
    for (auto &label : labelVec)
        labelFile << label << '\n';
    labelFile.close();
}
