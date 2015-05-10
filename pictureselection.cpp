#include "pictureselection.h"

using std::shared_ptr;
using std::cout;
using std::endl;

QString PictureSelection::path = "../pictures_selected/";

PictureSelection::PictureSelection()
{
}

void PictureSelection::select(QStringList files)
{
    for (QString fileSingle : files) {
        IplImage *pictureSingle = cvLoadImage(fileSingle.toLocal8Bit().data());
        auto picturePtrVec = analyse::process(pictureSingle, true);
        if (cvWaitKey(0) == '\r') {
            QStringList nameSpilted = fileSingle.split("/");
            QString nameWithoutPath = nameSpilted.at(nameSpilted.size() - 1);
            QString namePath = PictureSelection::path + nameWithoutPath;
            cout << namePath.toLocal8Bit().data() << endl;
            cvSaveImage(namePath.toLocal8Bit().data(), pictureSingle);
        }
        cvDestroyAllWindows();
        cvReleaseImage(&pictureSingle);
        for (auto &picturePtr : picturePtrVec)
            cvReleaseImage(&picturePtr);
    }
}
