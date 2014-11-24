#include "analyse.h"

analyse::analyse()
{
}

int analyse::analyseV(IplImage *img)
{
    IplImage* v = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    int max_index = 0;
    int hist_size = 256;
    float range[] = {0,255};
    float* ranges[]={range};
    CvHistogram* gray_hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);

    cvSplit(img, 0, 0, v, 0);
    cvCalcHist(&v, gray_hist, 0, 0);
    cvGetMinMaxHistValue(gray_hist, 0, 0, 0, &max_index);

    //qDebug("%d", max_index);
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
    //analyse::showImg(hist_image, "hist_image");
    return hist_image;

}

void analyse::formIE(const IplImage *Is, IplImage *Ig, IplImage *IE, int T)
{
    IplImage* IsDivIg = cvCreateImage(cvGetSize(Is), IPL_DEPTH_8U, 1);

    if (T > 250)//choose the T value according to the V-channel of the picture.
        T = 2;//there maybe exist a function could apply to all the picture to calculate the T value
    else if (T > 190)
        T = 4;
    else
        T = 45;

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
            *E = (*g < T ? *s: *sdivg);
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
    cvNot(temp, temp);
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

IplImage* analyse::analyseCoutours(IplImage *img)
{
    IplImage* pic = cvCreateImage(cvGetSize(img), 8, 1);
    cvCopy(img, pic);
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contours = 0;
    cvFindContours(pic, storage, &contours, sizeof(CvContour), CV_RETR_LIST);
    cvZero(pic);
    if (contours) {
        CvSeq* c = contours;
        //int i = 0;
        //analyse::showImg(pic, "testing");
       // cvWaitKey(0);
        for (; c != NULL; c = c->h_next) {
            if (cvContourArea(c) < 2000) continue;
            cvDrawContours(pic, c, cvScalar(255), cvScalarAll(125), 0, -1);
            //qDebug("i = %d, area = %f, perimeter = %f", ++i, cvContourArea(c), cvArcLength(c));
            //CvRect rect = cvBoundingRect(c,0);
           // cvRectangle(pic, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),cvScalarAll(255), 3, 8, 0);
            //analyse::showImg(pic, "testing");
            //if (cvWaitKey(0)== 27) break;
        }
        //analyse::showImg(pic, "testing");
    }
    return pic;
}

CvPoint analyse::cvtContour2Point(CvSeq* contour)
{
    int x, y, count, i;
    CvPoint point;
    CvPoint ptr_point[10];
    x = y = count = 0;
    for (i = 0; i < 6; ++i) {
        ptr_point[i].x = -1;
        ptr_point[i].y = -1;
    }
    cvCvtSeqToArray(contour, ptr_point);
    for (i = 0; i < 6; ++i) {
        if ((ptr_point[i].x > 0) && (ptr_point[i].y > 0)) {
            x += ptr_point[i].x;
            y += ptr_point[i].y;
             ++count;
        }
    }
    point.x = x / count;
    point.y = y / count;
    return point;
}

IplImage* analyse::flood(IplImage *img, IplImage *im)
{
    IplImage* em = cvCreateImage(cvGetSize(img), img->depth, 1);
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contours = 0;
    CvPoint point[10];

    cvZero(em);
    cvFindContours(img, storage, &contours, sizeof(CvContour), CV_RETR_LIST);
    cvDrawContours(em, contours, cvScalar(255), cvScalarAll(125), 2, -1);
    cvThreshold(em, em, 200, 150, CV_THRESH_BINARY);
    cvFindContours(im, storage, &contours, sizeof(CvContour), CV_RETR_CCOMP);
    if (contours) {
        CvSeq* c = contours;
        CvSeq* approxpoly = 0;

        for (; c != NULL; c = c->h_next) {
            if (cvContourArea(c) < 2000) continue;
            approxpoly = cvApproxPoly(c, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 30, 0);
            cvCvtSeqToArray(approxpoly, point);
            cvFloodFill(em, point[0], cvScalarAll(255), cvScalarAll(100), cvScalarAll(200));
            cvFloodFill(em, point[1], cvScalarAll(255), cvScalarAll(100), cvScalarAll(200));
        }
    }
    cvThreshold(em, em, 200, 255, CV_THRESH_BINARY);
    analyse::fillHole(em);
    return em;
}


IplImage* analyse::cvtIm2Waterseed(IplImage* img)
{
    IplImage* waterseed = cvCreateImage(cvGetSize(img), img->depth, 1);
    cvZero(waterseed);
    int i = 10;
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contours = 0;
    cvFindContours(img, storage, &contours, sizeof(CvContour), CV_RETR_CCOMP);
    if (contours) {
        CvSeq* c = contours;
        for (; c != NULL; c = c->h_next) {
            if (cvContourArea(c) < 2000) continue;//to be settled
            cvDrawContours(waterseed, c, cvScalarAll(i += 5), cvScalarAll(i), 0, -1);
        }
    }
    return waterseed;
}

cv::Mat analyse::water(IplImage* img, IplImage *imcopy)
{
    IplImage* waterseed = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* gradient = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* gradient2 =  cvCreateImage(cvGetSize(img), img->depth, 3);
    IplImage* imwaterseed = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* ero = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* notimg = cvCreateImage(cvGetSize(img), img->depth, 1);
    IplImage* backdist = cvCreateImage(cvGetSize(img), img->depth, 1);

    //generate gradient picture, erode and sub
    cvErode(img, ero, cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT), 3);
    cvSub(img, ero, gradient);
    cvCvtColor(gradient, gradient2, CV_GRAY2BGR);
    //analyse::showImg(gradient2, "gradient2");

    //generate imwaterseed, mark the nuclei with different color
    analyse::fillHole(imcopy);
    cvErode(imcopy, imcopy, cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT), 2);
    imwaterseed = analyse::cvtIm2Waterseed(imcopy);

    //generate background seed named backdist
    cvNot(img, notimg);
    cvDistTransform(notimg,  backdist, CV_DIST_L1);
    cvThreshold(backdist, backdist, 10, 255, CV_THRESH_BINARY);

    //generate waterseed, add the imwaterseed & backdist
    cvAdd(imwaterseed, backdist, waterseed);
    //analyse::showImg(waterseed, "waterseed");

    //watersheding
    cv::Mat image(gradient2, 0);
    cv::Mat imageMask(waterseed, 0);
    cv::Mat water;
    imageMask.convertTo(imageMask, CV_32S);
    cv::watershed(image, imageMask);
    imageMask.convertTo(water, CV_8U, 255, 255);
    return water;
}
