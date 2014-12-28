#include "nclseg.h"

nclseg::nclseg()
{
}

IplImage* nclseg::seg(IplImage* img)
{
    //int x, y, heigth, width;
    int isalot;
    IplImage* gray = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* BW1 = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* BJ = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* Ib = cvCreateImage(cvGetSize(img), img->depth, 1);
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
    IplImage* water3 = cvCreateImage(cvGetSize(img), img->depth, 3);
    IplImage* water3backup = cvCreateImage(cvGetSize(img), img->depth, 3);
    IplImage* luv = cvCreateImage(cvGetSize(img), img->depth, 3);
    IplImage* mean = cvCreateImage(cvGetSize(img), img->depth, 3);
    IplImage* waterb = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* water4 = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* water5 = cvCreateImage(cvGetSize(img), img->depth, 3);

//left side
    cvCvtColor(img, gray, CV_BGR2GRAY);
    cvCvtColor(img, HSV, CV_BGR2HSV);
    cvSplit(HSV, 0, Is, 0, 0);
    cvSplit(img, Ib, Ig, 0, 0);
    cvThreshold(gray, BW1, 180, 255, CV_THRESH_BINARY_INV);//125
    cvThreshold(gray, BJ, 0, 255, CV_THRESH_OTSU);
    cvThreshold(BJ, BJ, 100, 255, CV_THRESH_BINARY_INV);
    cvThreshold(Ib, BW3, analyse::Otsu(Ib), 255, CV_THRESH_BINARY_INV);
    cvSub(BW3, BW1, Redcell);
    cvSub(BJ, Redcell, BW2);

//right side
    cvEqualizeHist(Is, Is);
    cvEqualizeHist(Ig, Ig);
    analyse::formIE(Is, Ig, IE, analyse::analyseV(img));
    cvThreshold(IE, IE, 220, 255, CV_THRESH_BINARY);
    cvMorphologyEx(IE, IE, 0, cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT), CV_MOP_OPEN, 3);
    cvMorphologyEx(IE, IE, 0, cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT), CV_MOP_CLOSE, 10);
    analyse::fillHole(IE);
    analyse::showImg(IE, "IE");
    analyse::showImg(BW2, "BW2");

//middle algorithm
    cvAnd(BW2, IE, im);
    cvCopy(im, imcopy);//backup for watershed
    em = analyse::flood(BW2, im, &isalot);//get em and count nuclei to decide whether watershed or not
    analyse::showImg(em, "em");

//watershed ? or not?
    if (isalot) {
        water = analyse::water(BW2, imcopy);
        cvCopy(&IplImage(water), water2);
        cvAnd(em, water2, water2);
    }
    else
        cvCopy(em, water2);

    cvCvtColor(water2, water3, CV_GRAY2BGR);
    cvMin(water3, img, water3);
    cvCopy(water3, water3backup);
    analyse::showImg(water3, "water3");

////meanshift
//    cvCvtColor(water3, luv, CV_BGR2Luv);
//    cvPyrMeanShiftFiltering(luv, mean, 30, 30);
//    cvCvtColor(mean, water3, CV_Luv2BGR);
//    //analyse::showImg(water3, "water3-after");

    cvCvtColor(water3, waterb, CV_BGR2GRAY);
    //analyse::showImg(waterb, "waterb");
    cvThreshold(waterb, water4, analyse::Otsu2(waterb), 255, CV_THRESH_BINARY_INV);
    //analyse::showImg(water4, "water4");
    cvCvtColor(water4, water5, CV_GRAY2BGR);
    cvMin(water3backup, water5, water5);
    analyse::showImg(water5, "water5");


    cvWaitKey(0);
    cvDestroyAllWindows();
    cvSaveImage("wholeCell.jpg", water3);
    cvSaveImage("nuclei.jpg", water5);


    return img;
}

QStringList nclseg::seg4train(IplImage* img)
{
    int isalot;
    IplImage* gray = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* BW1 = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* BJ = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* Ib = cvCreateImage(cvGetSize(img), img->depth, 1);
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
    IplImage* water3 = cvCreateImage(cvGetSize(img), img->depth, 3);
    IplImage* water3backup = cvCreateImage(cvGetSize(img), img->depth, 3);
    IplImage* luv = cvCreateImage(cvGetSize(img), img->depth, 3);
    IplImage* mean = cvCreateImage(cvGetSize(img), img->depth, 3);
    IplImage* waterb = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* water4 = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* water5 = cvCreateImage(cvGetSize(img), img->depth, 3);

//left side
    cvCvtColor(img, gray, CV_BGR2GRAY);
    cvCvtColor(img, HSV, CV_BGR2HSV);
    cvSplit(HSV, 0, Is, 0, 0);
    cvSplit(img, Ib, Ig, 0, 0);
    cvThreshold(gray, BW1, 180, 255, CV_THRESH_BINARY_INV);//125
    cvThreshold(gray, BJ, 0, 255, CV_THRESH_OTSU);
    cvThreshold(BJ, BJ, 100, 255, CV_THRESH_BINARY_INV);
    cvThreshold(Ib, BW3, analyse::Otsu(Ib), 255, CV_THRESH_BINARY_INV);
    cvSub(BW3, BW1, Redcell);
    cvSub(BJ, Redcell, BW2);

//right side
    cvEqualizeHist(Is, Is);
    cvEqualizeHist(Ig, Ig);
    analyse::formIE(Is, Ig, IE, analyse::analyseV(img));
    cvThreshold(IE, IE, 220, 255, CV_THRESH_BINARY);
    cvMorphologyEx(IE, IE, 0, cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT), CV_MOP_OPEN, 3);
    cvMorphologyEx(IE, IE, 0, cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT), CV_MOP_CLOSE, 10);
    analyse::fillHole(IE);

//middle algorithm
    cvAnd(BW2, IE, im);
    cvCopy(im, imcopy);//backup for watershed
    em = analyse::flood(BW2, im, &isalot);//get em and count nuclei to decide whether watershed or not
    //qDebug("isalot:%d", isalot);

//watershed ? or not?
    if (isalot) {
        water = analyse::water(BW2, imcopy);
        cvCopy(&IplImage(water), water2);
        cvAnd(em, water2, water2);
    }
    else
        cvCopy(em, water2);

    cvCvtColor(water2, water3, CV_GRAY2BGR);
    cvMin(water3, img, water3);
    cvCopy(water3, water3backup);
    //analyse::showImg(water3, "water3");

////meanshift
//    cvCvtColor(water3, luv, CV_BGR2Luv);
//    cvPyrMeanShiftFiltering(luv, mean, 30, 30);
//    cvCvtColor(mean, water3, CV_Luv2BGR);
//    //analyse::showImg(water3, "water3-after");

    cvCvtColor(water3, waterb, CV_BGR2GRAY);
    //analyse::showImg(waterb, "waterb");
    cvThreshold(waterb, water4, analyse::Otsu2(waterb), 255, CV_THRESH_BINARY_INV);
    //analyse::showImg(water4, "water4");
    cvCvtColor(water4, water5, CV_GRAY2BGR);
    cvMin(water3backup, water5, water5);
    //analyse::showImg(water5, "water5");

////LBP
//    IplImage* fgray = cvCreateImage(cvGetSize(img), img->depth, 1);
//    IplImage* lbp = cvCreateImage(cvGetSize(img), img->depth, 1);
//    cvCvtColor(water5, fgray, CV_BGR2GRAY);
//    feature::LBP(fgray, lbp);
//    analyse::showImg(lbp, "lbp");



    return feature::selectFeature(water3, water5);
}
