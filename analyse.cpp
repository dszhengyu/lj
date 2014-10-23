#include "analyse.h"

analyse::analyse()
{
}

int analyse::analyseV(IplImage *img)
{
    IplImage* v = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    int max_index = 0;
    cvSplit(img, 0, 0, v, 0);
    analyseHist(v, &max_index);
    qDebug("%d", max_index);
    return max_index;
}

IplImage* analyse::analyseHist(IplImage *channel, int *ptr_max_index)
{
    int hist_size = 256;
    int hist_height = 256;
    float range[] = {0,255};
    float* ranges[]={range};
    CvHistogram* gray_hist = cvCreateHist(1,&hist_size,CV_HIST_ARRAY,ranges,1);
    cvCalcHist(&channel, gray_hist, 0, 0);
    cvNormalizeHist(gray_hist, 1.0);

    int scale = 2;
    IplImage* hist_image = cvCreateImage(cvSize(hist_size*scale,hist_height), 8, 3);
    cvZero(hist_image);
    float max_value = 0;
    cvGetMinMaxHistValue(gray_hist, 0, &max_value, 0, ptr_max_index);

    for(int i = 0;i < hist_size; i++)
    {
        float bin_val = cvQueryHistValue_1D(gray_hist, i);
        int intensity = cvRound(bin_val*hist_height/max_value);
        cvRectangle(hist_image,
            cvPoint(i*scale, hist_height-1),
            cvPoint((i+1)*scale - 1, hist_height - intensity),
            CV_RGB(255,255,255));
    }
    return hist_image;

}

IplImage* analyse::splictB(const IplImage* img)
{
    IplImage* b = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    IplImage* g = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    IplImage* r = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    cvSplit(img, b, g, r, 0);

    return b;
}

IplImage* analyse::splictS(const IplImage* img)
{
    IplImage* h = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    IplImage* s = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    IplImage* v = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    cvSplit(img, h, s, v, 0);

    return s;
}

IplImage* analyse::splictG(const IplImage* img)
{
    IplImage* b = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    IplImage* g = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    IplImage* r = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    cvSplit(img, b, g, r, 0);

    return g;
}

void analyse::formIE(const IplImage *Is, IplImage *Ig, IplImage *IE, int T)
{
    IplImage* IsDivIg = cvCreateImage(cvGetSize(Is), IPL_DEPTH_8U, 1);

    if (T > 200)
        T = 1;
    else
        T = 150;

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
            *E = (*g < T ? *s : *sdivg);
            ++E;
            ++g;
            ++s;
            ++sdivg;

        }
    }

}

IplImage* analyse::fillHole(IplImage *hole)
{
    IplImage* temp = cvCreateImage(cvGetSize(hole), IPL_DEPTH_8U, 1);
    cvCopy(hole, temp);
    cvFloodFill(temp, cvPoint(0, 0), cvScalarAll(255));
    showImg(temp, "temp");
    cvNot(temp, temp);
    showImg(temp, "temp2");
    cvAdd(hole, temp, hole);
    return hole;
}

int analyse::Otsu(IplImage* src)
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

void analyse::showImg(IplImage *img, char *s)
{
    cvNamedWindow(s, 0);
    cvShowImage(s, img);
}

