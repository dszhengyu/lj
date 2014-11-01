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
    //IplImage* circles = cvCreateImage(cvGetSize(img), img->depth, 1);
    //IplImage* ellipse = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* approxpoly = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* em = cvCreateImage(cvGetSize(img), img->depth, 1);
    //IplImage* em_after =  cvCreateImage(cvGetSize(img), img->depth, 3);
    IplImage* imcopy = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* waterseed = cvCreateImage(cvGetSize(img), img->depth, 1);


    struct point *Point;
    Point = (struct point *)malloc(sizeof(struct point));
    Point->ciclepoint = cvPoint(-1, -1), Point->next = NULL;

    analyse::showImg(img, "img");
    cvCvtColor(img, gray, CV_BGR2GRAY);
    cvThreshold(gray, BW1, 125, 255, CV_THRESH_BINARY_INV);
    cvThreshold(gray, BJ, analyse::Otsu(gray), 255, CV_THRESH_BINARY_INV);
    cvThreshold(analyse::splictB(img), BW3, analyse::Otsu(analyse::splictB(img)), 255, CV_THRESH_BINARY_INV);
    cvSub(BW3, BW1, Redcell);
    cvSub(BJ, Redcell, BW2);
    cvCvtColor(img, HSV, CV_BGR2HSV);
    Is = analyse::splictS(HSV);
    Ig = analyse::splictG(img);
    cvEqualizeHist(Is, Is);
    cvEqualizeHist(Ig, Ig);
    analyse::formIE(Is, Ig, IE, analyse::analyseV(img));
    cvThreshold(IE, IE, 220, 255, CV_THRESH_BINARY);
    cvMorphologyEx(IE, IE, 0, cvCreateStructuringElementEx(3, 3, 2, 2, CV_SHAPE_RECT), CV_MOP_OPEN, 3);
    cvMorphologyEx(IE, IE, 0, cvCreateStructuringElementEx(3, 3, 2, 2, CV_SHAPE_RECT), CV_MOP_CLOSE, 10);
    analyse::fillHole(IE);
    cvErode(IE, IE, cvCreateStructuringElementEx(3, 3, 2, 2, CV_SHAPE_RECT), 3);
    cvAnd(BW2, IE, im);
    cvCopy(im, imcopy);

    em = analyse::analyseCoutours(BW2);
    analyse::lighten(em);
    //analyse::showImg(em, "em");

    analyse::showImg(im, "im");
    analyse::analyseCoutours2ApproxPoly(im, approxpoly, Point);

    struct point *temp = Point;
    while (Point = Point->next) {
        //if (cvWaitKey(0) == 27) break;
       //qDebug("(%d, %d)", Point->ciclepoint.x, Point->ciclepoint.y);
        cvFloodFill(em, Point->ciclepoint, cvScalarAll(255), cvScalarAll(100), cvScalarAll(200));
        //analyse::showImg(em, "em");
    }
    cvWaitKey(0);
    cvThreshold(em, em, 200, 255, CV_THRESH_BINARY);

    analyse::fillHole(em);
    analyse::showImg(em, "em");

    //free the list
    struct point *temp1 = NULL;
    while ((temp1 = temp->next) != NULL) {
        free(temp);
        temp = temp1;
    }
    free(temp);

    analyse::fillHole(imcopy);
    //cvErode(imcopy, imcopy, cvCreateStructuringElementEx(3, 3, 2, 2, CV_SHAPE_RECT), 20);

    //waterseed = analyse::cvtContour2Waterseed(imcopy);
    waterseed = analyse::analyseCoutours2(imcopy);
    analyse::showImg(imcopy, "imcopy");
    analyse::showImg(waterseed, "waterseed");

   // cvCvtColor(em, em_after, CV_GRAY2BGR);
    //analyse::showImg(em_after, "em_after");

    analyse::showImg(im, "im-now");
    cv::Mat image(img, 0);
    cv::Mat imageMask(waterseed, 0);
    cv::Mat water;
    imageMask.convertTo(imageMask, CV_32S);
    cv::watershed(image, imageMask);
   // imageMask.convertTo(imageMask,CV_8U,255, 255);
    imageMask.convertTo(water, CV_8U);
    analyse::showImg(&IplImage(water), "water");

    //cvAnd(em, &IplImage(water), &IplImage(water));
    //analyse::showImg(&IplImage(water), "water-after");

    cvWaitKey(0);
    cvDestroyAllWindows();

    return IE;
}

IplImage* nclseg::water(IplImage* src, IplImage *dst)
{
    cv::Mat image(src, 0);
    cv::Mat imageMask(dst, 0);
    cv::Mat final;
    image.convertTo(image, CV_32S);
    imageMask.convertTo(imageMask, CV_32S);
    cv::watershed(image, imageMask);
    imageMask.convertTo(final, CV_8U);
    analyse::showImg(&IplImage(final), "water");
    return &IplImage(final);
}
