#include "gram_schmidt_seg.h"

gram_schmidt_seg::gram_schmidt_seg()
{
}

IplImage *gram_schmidt_seg::execGs(IplImage *img)
{
    IplImage* gray = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* BW1 = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* BJ = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* Ib = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* BW3 = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* Redcell = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* HSV = cvCreateImage(cvGetSize(img), img->depth, 3);
    IplImage* Is = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* Ig = cvCreateImage(cvGetSize(img), img->depth, 1);

    IplImage *red = cvCreateImage(cvGetSize(img), img->depth, 3);
    IplImage *bj = cvCreateImage(cvGetSize(img), img->depth, 3);
    IplImage *white = cvCreateImage(cvGetSize(img), img->depth, 3);

    cvCvtColor(img, gray, CV_BGR2GRAY);
    cvCvtColor(img, HSV, CV_BGR2HSV);
    cvSplit(HSV, 0, Is, 0, 0);
    cvSplit(img, Ib, Ig, 0, 0);
    cvThreshold(gray, BW1, 125, 255, CV_THRESH_BINARY_INV);//125
    cvThreshold(gray, BJ, 0, 255, CV_THRESH_OTSU);
    cvThreshold(BJ, BJ, 100, 255, CV_THRESH_BINARY_INV);
    cvThreshold(Ib, BW3, analyse::Otsu(Ib), 255, CV_THRESH_BINARY_INV);
    cvSub(BW3, BW1, Redcell);

    cvCvtColor(Redcell, red, CV_GRAY2BGR);
    cvCvtColor(BJ, bj, CV_GRAY2BGR);
    cvCvtColor(BW1, white, CV_GRAY2BGR);

    cvMin(img, red, red);
    cvMin(img, bj, bj);
    cvMin(img, white, white);

    analyse::showImg(img, "img");
    analyse::showImg(Redcell, "Redcell");
    analyse::showImg(BJ, "BJ");
    analyse::showImg(BW1, "BW1");
    analyse::showImg(red, "red");
    analyse::showImg(white, "white");

    cvWaitKey(0);
    cvDestroyAllWindows();

    cvReleaseImage(&gray);
    cvReleaseImage(&BW1);
    cvReleaseImage(&BJ);
    cvReleaseImage(&Ib);
    cvReleaseImage(&BW3);
    cvReleaseImage(&Redcell);
    cvReleaseImage(&HSV);
    cvReleaseImage(&Is);
    cvReleaseImage(&Ig);

}

CvScalar gram_schmidt_seg::meanBgr(IplImage *src)
{
    double b, g, r;
    int height=src->height;
    int width=src->width;
    int step = src->widthStep;
    int channels = src->nChannels;
    int size = 0;
    unsigned char *begin = (unsigned char*)src->imageData;
    b = 0;
    g = 0;
    r = 0;

    for(int i=0; i < height; i++)
    {
        unsigned char* p = begin + step * i;
        for(int j = 0; j < width; j++, p = p + channels)
        {
            if (p[0]) {
                b += p[0];
                g += p[1];
                r += p[2];
                size++;
            }
        }
    }
    b /= size;
    g /= size;
    r /= size;

    return cvScalar(b, g, r);
}

CvScalar gram_schmidt_seg::projection(CvScalar v, CvScalar u)
{
    double x, y;
    x = u.val[0] * v.val[0] + u.val[1] * v.val[1] + u.val[2] * v.val[2];
    y = u.val[0] * u.val[0] + u.val[1] * u.val[1] + u.val[2] * u.val[2];
    x /= y;
    return cvScalar(u.val[0] * x, u.val[1] * x, u.val[2] * x);
}

CvScalar gram_schmidt_seg::calW(CvScalar v1, CvScalar v2, CvScalar v3)
{
    CvScalar u1, u2, w, projv2u1, projv3u1, projv3u2;
    u1 = v1;
    projv2u1 = projection(v2, u1);
    u2 = cvScalar(v2.val[0] - projv2u1.val[0], v2.val[1] - projv2u1.val[1], v2.val[2] - projv2u1.val[2]);
    projv3u1 = projection(v3, u1);
    projv3u2 = projection(v3, u2);
    w = cvScalar(v3.val[0] - projv3u1.val[0] - projv3u2.val[0], v3.val[1] - projv3u1.val[1] - projv3u2.val[1], v3.val[2] - projv3u1.val[2] - projv3u2.val[2]);
    return w;
}

IplImage *gram_schmidt_seg::getFinal(CvScalar w, IplImage *src)
{
    IplImage *dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_64F, 1);
    int height=src->height;
    int width=src->width;
    int steps = src->widthStep;
    int stepd = dst->widthStep;
    int channels = src->nChannels;
    unsigned char *p = (unsigned char*)src->imageData;
    double *q = (double *)dst->imageData;
    double tmp = 0;

    for(int i=0; i < height; i++) {
        for(int j = 0; j < width; j++, p = p + channels, q++) {
            tmp = p[0] * w.val[0] + p[1] * w.val[1] + p[2] + w.val[2];
            *q = tmp;
        }
    }
    return dst;
}
