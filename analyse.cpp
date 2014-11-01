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

    if (T > 250)//choose the T value according to the V-channel of the picture.
        T = 2;//there maybe exist a function could apply to all the picture to calculate the T value
    else if (T > 200)
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
        //int key;
        CvSeq* c = contours;
        //int i = 0;
        //analyse::showImg(pic, "testing");
       // cvWaitKey(0);
        for (; c != NULL; c = c->h_next) {
            if (cvContourArea(c) < 2000) continue;
            cvDrawContours(pic, c, cvScalar(255), cvScalarAll(125), 0, -1);
            //qDebug("i = %d, area = %f, perimeter = %f", ++i, cvContourArea(c), cvArcLength(c));
            CvRect rect = cvBoundingRect(c,0);
            //cvRectangle(pic, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),cvScalarAll(255), 3, 8, 0);
            //analyse::showImg(pic, "testing");
            //if ((key = cvWaitKey(0))== 27) break;
        }
        //analyse::showImg(pic, "testing");
    }
    return pic;
}

IplImage* analyse::analyseCoutours2(IplImage *img)
{
    IplImage* pic = cvCreateImage(cvGetSize(img), 8, 1);
    cvCopy(img, pic);
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contours = 0;
    cvFindContours(pic, storage, &contours, sizeof(CvContour), CV_RETR_LIST);
    cvZero(pic);
    if (contours) {
        //int key;
        CvSeq* c = contours;
        //int i = 0;
        //analyse::showImg(pic, "testing");
       // cvWaitKey(0);
        for (; c != NULL; c = c->h_next) {
            if (cvContourArea(c) < 2000) continue;
            cvDrawContours(pic, c, cvScalar(255), cvScalarAll(125), 0, 1);
            //qDebug("i = %d, area = %f, perimeter = %f", ++i, cvContourArea(c), cvArcLength(c));
            CvRect rect = cvBoundingRect(c,0);
            //cvRectangle(pic, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),cvScalarAll(255), 3, 8, 0);
            //analyse::showImg(pic, "testing");
            //if ((key = cvWaitKey(0))== 27) break;
        }
        //analyse::showImg(pic, "testing");
    }
    return pic;
}

int analyse::analyseCoutours2Circle(IplImage* src, IplImage* dst, struct point* Point)
{
    struct circle {
        CvPoint2D32f center;
        float radius = 255;
    } a_circle;
    float rmin = 255;
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contours = 0;
    cvFindContours(src, storage, &contours, sizeof(CvContour), CV_RETR_CCOMP);
    cvZero(dst);
    if (contours) {
        int key;
        CvSeq* c = contours;
        analyse::showImg(dst, "circle");
        //cvWaitKey(0);
        for (; c != NULL; c = c->h_next) {
            if (cvContourArea(c) < 2000) continue;
            struct point *temp;
            temp = (struct point *)malloc(sizeof(struct point));
            Point->next = temp;
            Point = temp;
            cvDrawContours(dst, c, cvScalar(255), cvScalarAll(125), 0, -1);
            cvMinEnclosingCircle(c, &a_circle.center, &a_circle.radius);
            rmin = ((rmin < a_circle.radius) ? rmin : a_circle.radius);
            cvCircle(dst, Point->ciclepoint = cvPoint((int)a_circle.center.x,(int)a_circle.center.y), (int)a_circle.radius, cvScalarAll(255), -1);
           // analyse::showImg(dst, "circle");
            //qDebug("(%d, %d)", Point->ciclepoint.x, Point->ciclepoint.y);
            //if ((key = cvWaitKey(0))== 27) break;
        }
        Point->next = NULL;

    }
    analyse::showImg(dst, "circle");
    //qDebug("%f", rmin);
    return (int)rmin;
}

void analyse::analyseCoutours2Ellipse(IplImage* src, IplImage* dst, struct point *Point)
{
    CvBox2D box;
    int x, y;
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contours = 0;
    cvFindContours(src, storage, &contours, sizeof(CvContour), CV_RETR_CCOMP);
    cvZero(dst);
    if (contours) {
        int key;
        CvSeq* c = contours;
        analyse::showImg(dst, "ellipse");
        cvWaitKey(0);
        for (; c != NULL; c = c->h_next) {
            if (cvContourArea(c) < 2000) continue;
            cvDrawContours(dst, c, cvScalar(255), cvScalarAll(125), 0, -1);

            //insert the point in the list
            struct point *temp;
            temp = (struct point *)malloc(sizeof(struct point));
            Point->next = temp;
            Point = temp;
            box = cvFitEllipse2(c);
            x = ((int)box.center.x > dst->width ? dst->width - 3: (int)box.center.x);//x and y may out of range!
            y = ((int)box.center.y > dst->height ? dst->height - 3: (int)box.center.y);
            Point->ciclepoint = cvPoint(x, y);

            //draw the ellipse in the dst
            cvEllipseBox(dst, box, cvScalarAll(255), 3);
            analyse::showImg(dst, "ellipse");
            qDebug("(%d, %d)", Point->ciclepoint.x, Point->ciclepoint.y);
            //if ((key = cvWaitKey(0))== 27) break;
        }
        Point->next = NULL;

    }
    analyse::showImg(dst, "ellipse");
}

void analyse::analyseCoutours2ApproxPoly(IplImage* src, IplImage* dst, struct point *Point)
{
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvMemStorage* storage2 = cvCreateMemStorage(0);
    CvSeq* contours = 0;
    cvFindContours(src, storage, &contours, sizeof(CvContour), CV_RETR_CCOMP);
    cvZero(dst);
    if (contours) {
        //int key;
        CvSeq* c = contours;
        CvSeq* approxpoly = 0;

        struct point *temp;
        for (; c != NULL; c = c->h_next) {
            if (cvContourArea(c) < 2000) continue;
            cvDrawContours(dst, c, cvScalar(255), cvScalarAll(125), 0, 1);
            approxpoly = cvApproxPoly(c, sizeof(CvContour), storage2, CV_POLY_APPROX_DP, 30, 0);
            cvDrawContours(dst, approxpoly, cvScalar(255), cvScalarAll(125), 0, 5);
            //insert the point in the list
            temp = (struct point *)malloc(sizeof(struct point));
            if(temp != NULL) {
                Point->next = temp;
                Point = temp;
            }
            else qDebug("OUT OF SPACE");
            Point->ciclepoint = analyse::cvtContour1Point(approxpoly);

            temp = (struct point *)malloc(sizeof(struct point));
            if(temp != NULL) {
                Point->next = temp;
                Point = temp;
            }
            else qDebug("OUT OF SPACE");
            Point->ciclepoint = analyse::cvtContour11Point(approxpoly);
            //analyse::showImg(dst, "approxpoly");
           // if ((key = cvWaitKey(0))== 27) break;
        }

        Point->next = NULL;
        analyse::showImg(dst, "approxpoly");

    }
}

//Attention ! unused!
IplImage* analyse::analyseCoutours1by1(IplImage* img)
{
    IplImage* pic = cvCreateImage(cvGetSize(img), 8, 1);
    cvCopy(img, pic);
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contours = 0;
    CvContourScanner scanner = cvStartFindContours(pic, storage);
    cvZero(pic);
    int key;
    do {
        if ((key = cvWaitKey(0))== 27) break;
        contours = cvFindNextContour(scanner);
        cvDrawContours(pic, contours, cvScalarAll(255), cvScalarAll(255), 0, 3, -1);
        CvRect rect = cvBoundingRect(contours,0);
        cvRectangle(pic, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),cvScalarAll(255), 2, 8, 0);
        analyse::showImg(pic, "testing");
        qDebug("there");
    } while (contours);
    cvEndFindContours(&scanner);

    return pic;
}

/*============================================================================
=  代码内容：最大熵阈值分割
=  修改日期:2009-3-3
=  作者:crond123
=  博客:http://blog.csdn.net/crond123/
=  E_Mail:crond123@163.com
===============================================================================
// 计算当前位置的能量熵
double caculateCurrentEntropy(CvHistogram * Histogram1,int cur_threshold,int state)
{
int start,end;
int  total =0;
double cur_entropy =0.0;
if(state == 0)
    {
        start =0;
        end = cur_threshold;
    }
else
    {
        start = cur_threshold;
        end =256;
    }
for(int i=start;i<end;i++)
    {
        total += (int)cvQueryHistValue_1D(Histogram1,i);//查询直方块的值 P304
    }
for(int j=start;j<end;j++)
    {
if((int)cvQueryHistValue_1D(Histogram1,j)==0)
continue;
double percentage = cvQueryHistValue_1D(Histogram1,j)/total;
//熵的定义公式
        cur_entropy +=-percentage*logf(percentage);
//根据泰勒展式去掉高次项得到的熵的近似计算公式
        //cur_entropy += percentage*percentage;
    }
return cur_entropy;
//    return (1-cur_entropy);
}

//寻找最大熵阈值并分割
void  analyse::MaxEntropy(IplImage *src,IplImage *dst)
{
    assert(src != NULL);
    assert(src->depth ==8&& dst->depth ==8);
    assert(src->nChannels ==1);
    int HistogramBins = 256;
    float range[] = {0,255};
    float* ranges[]={range};
    CvHistogram * hist  = cvCreateHist(1,&HistogramBins,CV_HIST_ARRAY,ranges);//创建一个指定尺寸的直方图
//参数含义：直方图包含的维数、直方图维数尺寸的数组、直方图的表示格式、方块范围数组、归一化标志
    cvCalcHist(&src,hist);//计算直方图
double maxentropy =-1.0;
int max_index =-1;
// 循环测试每个分割点，寻找到最大的阈值分割点
for(int i=0;i<HistogramBins;i++)
    {
double cur_entropy = caculateCurrentEntropy(hist,i,1)+caculateCurrentEntropy(hist,i,0);
if(cur_entropy>maxentropy)
        {
            maxentropy = cur_entropy;
            max_index = i;
        }
    }
    cvThreshold(src, dst, (double)max_index,255, CV_THRESH_BINARY);
    cvReleaseHist(&hist);
}

*/

int analyse::analyseMaxvalue(IplImage* img)
{
    int height, width, max;
    height = img->height;
    width = img->width;
    max = 0;
    for (int i = 0; i < height; ++i) {
        int key = i * img->widthStep;
        for (int j = 0; j < width; ++j) {
            int k = img->imageData[j + key];
            max = (max > k) ? max : k;
        }
    }
    qDebug("%d", max);
    return max;
}

void analyse::lighten(IplImage* img)
{
    int height, width, step, key;
    height = img->height;
    width = img->width;
    step = img->widthStep;
    for (int i = 0; i < height; ++i) {
        key = i * step;
        for (int j = 0; j < width; ++j) {
            int data = img->imageData[j + key];
            data = (data ? 150 : 0);
            img->imageData[j + key] = data;
        }
    }
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

CvPoint analyse::cvtContour1Point(CvSeq *contour)
{
    CvPoint point;
    CvPoint ptr_point[10];
    cvCvtSeqToArray(contour, ptr_point);
    point.x = ptr_point[0].x;
    point.y = ptr_point[0].y;
    return point;
}

CvPoint analyse::cvtContour11Point(CvSeq *contour)
{
    CvPoint point;
    CvPoint ptr_point[10];
    cvCvtSeqToArray(contour, ptr_point);
    point.x = ptr_point[1].x;
    point.y = ptr_point[1].y;
    return point;
}

IplImage* analyse::cvtContour2Waterseed(IplImage* img)
{
    IplImage* waterseed = cvCreateImage(cvGetSize(img), img->depth, 1);
    cvZero(waterseed);
    CvBox2D box;
    int x, y;
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contours = 0;
    cvFindContours(img, storage, &contours, sizeof(CvContour), CV_RETR_CCOMP);
    if (contours) {
        CvSeq* c = contours;
        //cvWaitKey(0);
        for (; c != NULL; c = c->h_next) {
            if (cvContourArea(c) < 2000) continue;

            box = cvFitEllipse2(c);
            x = ((int)box.center.x > img->width ? img->width - 3: (int)box.center.x);//x and y may out of range!
            y = ((int)box.center.y > img->height ? img->height - 3: (int)box.center.y);

            cvCircle(waterseed, cvPoint(x, y), 1, cvScalarAll(255));

            qDebug("(%d, %d)", x, y);
            //if (cvWaitKey(0)== 27) break;
        }

    }
    analyse::showImg(waterseed, "waterseed");
    return waterseed;
}
