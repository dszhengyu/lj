#include "nclseg.h"

nclseg::nclseg()
{
}

IplImage* nclseg::seg(IplImage* img)
{
    IplImage* gray = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* BW1 = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* BJ = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* BW3 = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* Redcell = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* BW2 = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* HSV = cvCreateImage(cvGetSize(img), img->depth, 3);
    IplImage* Is = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* Ig = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* IE = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* im = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* imcopy = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* em = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* water2 = cvCreateImage(cvGetSize(img), img->depth, 1);
    cv::Mat water;

    analyse::showImg(img, "img");
    cvCvtColor(img, gray, CV_BGR2GRAY);
    cvThreshold(gray, BW1, 125, 255, CV_THRESH_BINARY_INV);
    cvThreshold(gray, BJ, 0, 255, CV_THRESH_OTSU);
    cvThreshold(BJ, BJ, 100, 255, CV_THRESH_BINARY_INV);
    cvThreshold(analyse::splictB(img), BW3, 0, 255, CV_THRESH_OTSU);
    cvThreshold(analyse::splictB(img), BW3, 100, 255, CV_THRESH_BINARY_INV);
    cvSub(BW3, BW1, Redcell);
    cvSub(BJ, Redcell, BW2);
    cvCvtColor(img, HSV, CV_BGR2HSV);
    Is = analyse::splictS(HSV);
    Ig = analyse::splictG(img);
    cvEqualizeHist(Is, Is);
    cvEqualizeHist(Ig, Ig);
    analyse::formIE(Is, Ig, IE, analyse::analyseV(img));
    cvThreshold(IE, IE, 220, 255, CV_THRESH_BINARY);
    cvMorphologyEx(IE, IE, 0, cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT), CV_MOP_OPEN, 3);
    cvMorphologyEx(IE, IE, 0, cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT), CV_MOP_CLOSE, 10);
    analyse::fillHole(IE);
    cvErode(IE, IE, cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT), 3);
    cvAnd(BW2, IE, im);
    cvCopy(im, imcopy);
    em = analyse::flood(BW2, im);
    water = analyse::water(BW2, imcopy);
    cvCopy(&IplImage(water), water2);
    cvAnd(em, water2, water2);
    analyse::showImg(water2, "water2");

    cvWaitKey(0);
    cvDestroyAllWindows();

    return water2;
}
