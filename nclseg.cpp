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
    IplImage* S = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* Is = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* G = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* Ig = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* IE = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* e = cvCreateImage(cvGetSize(img), img->depth, 1);


    cvCvtColor(img, gray, CV_BGR2GRAY);
    cvThreshold(gray, BW1, 125, 255, CV_THRESH_BINARY_INV);
    cvThreshold(gray, BJ, Otsu(gray), 255, CV_THRESH_BINARY_INV);
    cvThreshold(nclseg::Bsplict(img), BW3, Otsu(nclseg::Bsplict(img)), 255, CV_THRESH_BINARY_INV);
    cvSub(BW3, BW1, Redcell);
    cvSub(BJ, Redcell, BW2);
    cvCvtColor(img, HSV, CV_BGR2HSV);
    S = nclseg::Ssplict(HSV);
    G = nclseg::Gsplict(img);
    cvEqualizeHist(S, Is);
    cvEqualizeHist(G, Ig);
    formIE(Is, Ig, IE);
    cvThreshold(IE, e, 240, 255, CV_THRESH_BINARY);

    return e;
}

int nclseg::Otsu(IplImage* src)
{
    int height=src->height;
    int width=src->width;

    //histogram
    float histogram[256] = {0};
    for(int i=0; i < height; i++)
    {
        unsigned char* p=(unsigned char*)src->imageData + src->widthStep * i;
        for(int j = 0; j < width; j++)
        {
            histogram[*p++]++;
        }
    }
    //normalize histogram
    int size = height * width;
    for(int i = 0; i < 256; i++)
    {
        histogram[i] = histogram[i] / size;
    }

    //average pixel value
    float avgValue=0;
    for(int i=0; i < 256; i++)
    {
        avgValue += i * histogram[i];  //整幅图像的平均灰度
    }

    int threshold;
    float maxVariance=0;
    float w = 0, u = 0;
    for(int i = 0; i < 256; i++)
    {
        w += histogram[i];  //假设当前灰度i为阈值, 0~i 灰度的像素(假设像素值在此范围的像素叫做前景像素) 所占整幅图像的比例
        u += i * histogram[i];  // 灰度i 之前的像素(0~i)的平均灰度值： 前景像素的平均灰度值

        float t = avgValue * w - u;
        float variance = t * t / (w * (1 - w) );
        if(variance > maxVariance)
        {
            maxVariance = variance;
            threshold = i;
        }
    }

    return threshold;
}

IplImage* nclseg::Bsplict(const IplImage* img)
{
    IplImage* b = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    IplImage* g = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    IplImage* r = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    cvSplit(img, b, g, r, 0);

    return b;
}

IplImage* nclseg::Ssplict(const IplImage* img)
{
    IplImage* h = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    IplImage* s = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    IplImage* v = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    cvSplit(img, h, s, v, 0);

    return s;
}

IplImage* nclseg::Gsplict(const IplImage* img)
{
    IplImage* b = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    IplImage* g = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    IplImage* r = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    cvSplit(img, b, g, r, 0);

    return g;
}

void nclseg::formIE(const IplImage *Is, IplImage *Ig, IplImage *IE)
{
    IplImage* IsDivIg = cvCreateImage(cvGetSize(Is), IPL_DEPTH_8U, 1);
    unsigned char T = (unsigned char)Otsu(Ig);

    //T!!!!

    T = 230;
    int height= IE->height;
    int width = IE->width;

    cvDiv(Is, Ig, IsDivIg);

    for(int i = 0; i < height; i++)
    {
        unsigned char* E=(unsigned char*)IE->imageData + IE->widthStep * i;
        unsigned char* g=(unsigned char*)Ig->imageData + Ig->widthStep * i;
        unsigned char* s=(unsigned char*)Is->imageData + Is->widthStep * i;
        unsigned char* sdivg=(unsigned char*)IsDivIg->imageData + IsDivIg->widthStep * i;
        for(int j = 0; j < width; j++)
        {
            *E = (*g > T ? *s : *sdivg);
            *E = (*g > T ? 0 : *sdivg);
            ++E;
            ++g;
            ++s;
            ++sdivg;
        }
    }

}
