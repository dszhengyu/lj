#include "analyse.h"

extern int waitTime;

analyse::analyse()
{
}

vector<IplImage *> analyse::process(IplImage* img, bool debug)
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
    IplImage* Iv = cvCreateImage(cvGetSize(img), img->depth, 1);
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

    //left side，最后需要BW2即可，其他释放
        cvCvtColor(img, gray, CV_BGR2GRAY);
        cvCvtColor(img, HSV, CV_BGR2HSV);
        cvSplit(HSV, 0, Is, Iv, 0);
        cvSplit(img, Ib, Ig, 0, 0);
        cvThreshold(gray, BW1, 125, 255, CV_THRESH_BINARY_INV);//125
        cvThreshold(gray, BJ, 0, 255, CV_THRESH_OTSU);
        cvThreshold(BJ, BJ, 100, 255, CV_THRESH_BINARY_INV);
        cvThreshold(Ib, BW3, analyse::Otsu(Ib), 255, CV_THRESH_BINARY_INV);
        cvSub(BW3, BW1, Redcell);
        cvSub(BJ, Redcell, BW2);


//        analyse::showImg(img, "img");
//        analyse::showImg(Redcell, "Redcell");
//        analyse::showImg(BJ, "BJ");
//        analyse::showImg(BW1, "BW1");
//        analyse::showImg(Iv, "Iv");


        cvReleaseImage(&gray);
        cvReleaseImage(&BW1);
        cvReleaseImage(&BJ);
        cvReleaseImage(&Ib);
        cvReleaseImage(&BW3);
        cvReleaseImage(&Redcell);
        cvReleaseImage(&HSV);

    //right side，最后需要IE即可，其它释放
        cvEqualizeHist(Is, Is);
        cvEqualizeHist(Ig, Ig);
        analyse::formIE(Is, Ig, IE, analyse::analyseV(img));
        cvThreshold(IE, IE, 220, 255, CV_THRESH_BINARY);
        cvMorphologyEx(IE, IE, 0, cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT), CV_MOP_OPEN, 3);
        cvMorphologyEx(IE, IE, 0, cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT), CV_MOP_CLOSE, 10);
        analyse::fillHole(IE);

        cvReleaseImage(&Is);
        cvReleaseImage(&Ig);


    //middle algorithm，留BW2,em，imcopy
        cvAnd(BW2, IE, im);
        cvCopy(im, imcopy);//backup for watershed
        em = analyse::flood(BW2, im, isalot);//get em and count nuclei to decide whether watershed or not

        cvReleaseImage(&IE);
        cvReleaseImage(&im);

    //watershed ? or not?，只留water3
        if (isalot) {
            water = analyse::water(BW2, imcopy);
            //cvCopy(&IplImage(water), water2);
            cvAnd(em, water2, water2);
        }
        else
            cvCopy(em, water2);

        cvCvtColor(water2, water3, CV_GRAY2BGR);
        cvMin(water3, img, water3);
        cvCopy(water3, water3backup);
        //analyse::showImg(water3, "water3");

        cvReleaseImage(&BW2);
        cvReleaseImage(&em);
        cvReleaseImage(&imcopy);
        cvReleaseImage(&water2);

//meanshift
//        cvCvtColor(water3, luv, CV_BGR2Luv);
//        cvPyrMeanShiftFiltering(luv, mean, 30, 30);
//        cvCvtColor(mean, water3, CV_Luv2BGR);
    //    //analyse::showImg(water3, "water3-after");

        cvCvtColor(water3, waterb, CV_BGR2GRAY);
        //analyse::showImg(waterb, "waterb");
        cvThreshold(waterb, water4, analyse::Otsu2(waterb), 255, CV_THRESH_BINARY_INV);
        //analyse::showImg(water4, "water4");
        cvCvtColor(water4, water5, CV_GRAY2BGR);
        cvMin(water3backup, water5, water5);
        //analyse::showImg(water5, "water5");

        cvReleaseImage(&waterb);
        cvReleaseImage(&water3backup);
        cvReleaseImage(&water4);
        cvReleaseImage(&luv);
        cvReleaseImage(&mean);

//debug mode, display pic
        if (debug == true) {
            analyse::showImg(water3, "wholeCell");
            analyse::showImg(water5, "nuclei");
            //cvWaitKey(waitTime);
        }

    cvSaveImage("wholeCell.png", water3);
    cvSaveImage("nuclei.png", water5);

//should be released by caller, consider use smart-ptr later!
//    cvReleaseImage(&water3);
//    cvReleaseImage(&water5);

    return vector<IplImage*> {water3, water5};
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

    cvReleaseHist(&gray_hist);
    cvReleaseImage(&v);
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
                    cvPoint((i+1)*scale - 1, hist_height - intensity), CV_RGB(255,255,255));
    }
    //analyse::showImg(hist_image, "hist_image");

    cvReleaseHist(&gray_hist);
    cvReleaseImage(&hist_image);
    return hist_image;
}

void analyse::formIE(const IplImage *Is, IplImage *Ig, IplImage *IE, int T)
{
    IplImage* IsDivIg = cvCreateImage(cvGetSize(Is), IPL_DEPTH_8U, 1);

    if (T > 250)//choose the T value according to the V-channel of the picture.
        T = 1;//there maybe exist a function could apply to all the picture to calculate the T value
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
    cvReleaseImage(&IsDivIg);
}

void analyse::fillHole(IplImage *hole)
{
    IplImage* temp = cvCreateImage(cvGetSize(hole), IPL_DEPTH_8U, 1);
    cvCopy(hole, temp);
    cvFloodFill(temp, cvPoint(0, 0), cvScalarAll(255));
    cvNot(temp, temp);
    cvAdd(hole, temp, hole);

    cvReleaseImage(&temp);
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
    //1.5.2015   I think this is not normalize? It calculate the probility the pixel come up
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

int analyse::Otsu2(IplImage* src) //ignore 0
{
    int height=src->height;
    int width=src->width;
    int size = 0;
    //histogram
    float histogram[256] = {0};
    for(int i=0; i < height; i++)
    {
        unsigned char* p=(unsigned char*)src->imageData + src->widthStep * i;
        for(int j = 0; j < width; j++)
        {
            if (*p) {
                histogram[*p++]++;
                size++;
            }
            else {
                p++;
                continue;
            }
        }
    }

    //normalize histogram

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
    //qDebug("%d", threshold);
    threshold = (threshold > 80 ?threshold: 100);
    return threshold;
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

    cvReleaseMemStorage(&storage);

    return pic;
}

//caution!!! this fun may suffer from stackoverflow!!!
IplImage* analyse::flood(IplImage *img, IplImage *im, int &isalot)
{
    IplImage* em = cvCreateImage(cvGetSize(img), img->depth, 1);
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contours = 0;
//here is the problem!
    CvPoint point[20];
    int count = 0;
    cvCopy(img, em);
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
            count++;
        }
    }
    cvThreshold(em, em, 200, 255, CV_THRESH_BINARY);
    analyse::fillHole(em);
    isalot = (count > 5 ? 1 : 0);
    cvReleaseMemStorage(&storage);
    return em;
}


IplImage* analyse::cvtIm2Waterseed(IplImage* img)
{
    IplImage* waterseed = cvCreateImage(cvGetSize(img), img->depth, 1);
    cvZero(waterseed);
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contours = 0;
    cvFindContours(img, storage, &contours, sizeof(CvContour), CV_RETR_CCOMP);
    if (contours) {
        CvSeq* c = contours;
        for (int i = 10; c != NULL; c = c->h_next) {
            if (cvContourArea(c) < 2000) continue;//to be settled
            i += 5;
            cvDrawContours(waterseed, c, cvScalarAll(i), cvScalarAll(i), 0, -1);
        }
    }

    cvReleaseMemStorage(&storage);
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

    cvReleaseImage(&waterseed);
    cvReleaseImage(&gradient);
    cvReleaseImage(&gradient2);
    cvReleaseImage(&imwaterseed);
    cvReleaseImage(&ero);
    cvReleaseImage(&notimg);
    cvReleaseImage(&backdist);
//    image.~Mat();
//    imageMask.~Mat();

    return water;
}
