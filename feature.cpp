#include "feature.h"

extern int waitTime;

feature::feature()
{

}

feature::vec2Ddouble feature::calFeatureVec(IplImage* eachImage, bool debug)
{
    auto imgVec = analyse::process(eachImage, debug);

    //准备图片，整个细胞bgr， gray，轮廓分析备份，  细胞核bgr， gray，
    IplImage* wholeCell = imgVec[0];
    IplImage* wholeCellb = cvCreateImage(cvGetSize(wholeCell), 8, 1);
    IplImage* wholeCell4Contour= cvCreateImage(cvGetSize(wholeCell), 8, 1);
    IplImage* nuclei = imgVec[1];
    IplImage* nucleib = cvCreateImage(cvGetSize(nuclei), 8, 1);

    cvCvtColor(wholeCell, wholeCellb, CV_BGR2GRAY);
    cvCopy(wholeCellb, wholeCell4Contour);
    cvCvtColor(nuclei, nucleib, CV_BGR2GRAY);

//    analyse::showImg(wholeCell, "wholeCell");
//    analyse::showImg(nuclei, "nuclei");
//    cvWaitKey(0);


    //返回值， n * dimension 维向量
    feature::vec2Ddouble features;

    //对整个细胞gray图进行轮廓分析，找出一个个细胞，然后ROI， 再接着处理
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contours = 0;
    cvFindContours(wholeCell4Contour, storage, &contours, sizeof(CvContour), CV_RETR_LIST);
    if (contours) {
        CvSeq* c = contours;
        //int i = 0;
        for (; c != NULL; c = c->h_next) {
            if (cvContourArea(c) < 2000) continue;
            //qDebug("i = %d, area = %f, perimeter = %f", ++i, cvContourArea(c), cvArcLength(c));

            //开始设置ROI
            CvRect rect = cvBoundingRect(c,0);
            cvSetImageROI(wholeCell, rect);
            cvSetImageROI(wholeCellb, rect);
            cvSetImageROI(nuclei, rect);
            cvSetImageROI(nucleib, rect);
            //为临时图片申请空间，复制图片
            IplImage* temp1 = cvCreateImage(cvGetSize(nuclei), 8, 3);//细胞核bgr
            IplImage* temp2 = cvCreateImage(cvGetSize(nucleib), 8, 1);//细胞核gray
            IplImage* temp22 = cvCreateImage(cvGetSize(nucleib), 8, 1);//细胞核gray
            IplImage* temp3 = cvCreateImage(cvGetSize(wholeCell), 8, 3);//整个细胞bgr
            IplImage* temp4 = cvCreateImage(cvGetSize(wholeCellb), 8, 1);//整个细胞gray
            IplImage* temp5 = cvCreateImage(cvGetSize(wholeCell), 8, 3);//细胞质bgr
            cvCopy(nuclei, temp1);
            cvCopy(nucleib, temp2);
            cvCopy(nucleib, temp22);
            cvCopy(wholeCell, temp3);
            cvCopy(wholeCellb, temp4);
            //细胞质图片
            cvSub(temp3, temp1, temp5);

            //取消ROI设置
            cvResetImageROI(wholeCell);
            cvResetImageROI(wholeCellb);
            cvResetImageROI(nuclei);
            cvResetImageROI(nucleib);

            //开始提取特征， 用tempX图片
            auto t1 = feature::meanRgb(temp1);
            auto t3 = feature::meanRgb(temp3);
            auto t5 = feature::meanRgb(temp5);

            auto t2 = feature::getPAR(temp2, 2);//4-d
            auto t4 = feature::getPAR(temp4, 4);//3-d
            auto t6 = vector<double> ({t2.at(1) - t2.at(1), t2.at(1) / t2.at(1)});
            auto t22 = feature::getLBP(temp22);

            //细胞核面积为0，说明ROI设置有错，跳过，特征值不要啦
            if (t2.at(1) == 0) {
                continue;
            }

//debug mode, display the pic
            if (debug == true)  {
                analyse::showImg(temp1, "Nuclei-bgr");
                analyse::showImg(temp3, "Cell-bgr");
                analyse::showImg(temp5, "Cryto-bgr");
                cvWaitKey(waitTime * 2);
             }

            //如果细胞质面积为0，说明没有细胞质， 应该对BGR分量另外处理
            vector<double> eachFeature;
            auto insertItr = std::back_inserter(eachFeature);
            move(t2.begin(), t2.end(), insertItr);
            move(t1.begin(), t1.end(), insertItr);
            move(t4.begin(), t4.end(), insertItr);
            move(t3.begin(), t3.end(), insertItr);

            if (t6.at(0) == 0) {
                eachFeature.push_back(0);
                eachFeature.push_back(0);
                eachFeature.push_back(0);
            }
            else {
                move(t5.begin(), t5.end(), insertItr);
//                eachFeature << t2.join("'") << t1.join("'") << t4.join("'") << t3.join("'") << t5.join("'") << t6.join("'") << t22.join("'");
            }

            move(t6.begin(), t6.end(), insertItr);
            move(t22.begin(), t22.end(), insertItr);

            //把当前细胞的特征值放入图片总特征值中
            features.push_back(eachFeature);

//            analyse::showImg(temp1, "temp1");
//            analyse::showImg(temp3, "temp3");
//            if (cvWaitKey(0)== 27) break;


            cvReleaseImage(&temp1);
            cvReleaseImage(&temp2);
            cvReleaseImage(&temp22);
            cvReleaseImage(&temp3);
            cvReleaseImage(&temp4);
            cvReleaseImage(&temp5);
        }
    }
    cvReleaseMemStorage(&storage);
    cvReleaseImage(&wholeCell);
    cvReleaseImage(&wholeCellb);
    cvReleaseImage(&wholeCell4Contour);
    cvReleaseImage(&nuclei);
    cvReleaseImage(&nucleib);

    return features;

}

int feature::getHop(unsigned char i)
{
    int a[8] = {0};
    int k = 7;
    int cnt = 0;

    while(i) {
        a[k] = i & 1;
        i >>= 1;
        k--;
    }

    for (k = 0; k < 8; k++) {
        if (a[k] != a[k == 7 ? 0 : k + 1])
            cnt++;
    }

    return cnt;
}

void feature::formTable(unsigned char *table) // have some doubt
{
    memset(table, 0, 256);
    unsigned char temp = 1;

    for (int i = 0; i < 256; i++) {
        if (getHop(i) <= 2) {
            table[i] = temp;
            temp++;
        }
    }
}

vector<double> feature::getLBP(IplImage *src)
{
    IplImage* dst = cvCreateImage(cvGetSize(src), 8, 1);
    double histogram[256] = {0};
    int size = 0;

    int width = src->width;
    int height = src->height;
    int i, j;
    unsigned char table[256];
    formTable(table);

    for (i = 1; i < height - 1; i++) {
        for (j = 0; j < width - 1; j++) {
            unsigned char center = CV_IMAGE_ELEM(src, unsigned char, i, j);
            if (center == 0)
                continue;
            unsigned char neighborhood[8] = {0};
            neighborhood[7]	= CV_IMAGE_ELEM(src, unsigned char, i - 1, j - 1);
            neighborhood[6]	= CV_IMAGE_ELEM(src, unsigned char, i-1, j);
            neighborhood[5]	= CV_IMAGE_ELEM(src, unsigned char, i-1, j + 1);
            neighborhood[4]	= CV_IMAGE_ELEM(src, unsigned char, i, j + 1);
            neighborhood[3]	= CV_IMAGE_ELEM(src, unsigned char, i + 1, j + 1);
            neighborhood[2]	= CV_IMAGE_ELEM(src, unsigned char, i + 1, j);
            neighborhood[1]	= CV_IMAGE_ELEM(src, unsigned char, i + 1, j - 1);
            neighborhood[0]	= CV_IMAGE_ELEM(src, unsigned char, i, j - 1);
            unsigned char temp=0;

            for(int k = 0; k < 8; k++) {
                temp += (neighborhood[k] >= center) << k;
            }
            CV_IMAGE_ELEM(dst, unsigned char, i, j) = table[temp];

            //insert into histogram
            histogram[table[temp]]++;
        }
    }

    double mean, energy, entropy, variance;
    mean = variance = energy = entropy = 0;
    size = width * height;
    for(i = 0; i < 256; i++)
    {
        histogram[i] = histogram[i] / size;
    }

    for(i = 0; i < 256; i++)
    {
        mean += i * histogram[i];
        energy += sqrt(histogram[i]);
        //entropy -= histogram[i] * log2(histogram[i]);
    }

    for (i = 0; i < 256; i++)
        variance += (i - mean) * (i - mean) * histogram[i];

    //qDebug("entropy: %lf, variance: %lf", entropy, variance); 
    vector<double> lbpFeature({mean, energy, variance});

    //analyse::showImg(dst, "lbp");
    cvReleaseImage(&dst);

    //qDebug("lbp: %s", lbpFeature.join("'").toLocal8Bit().data());
    return lbpFeature;
}


vector<double> feature::meanRgb(IplImage *src)
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

    //qDebug("%lf, %lf, %lf", b, g, r);
    vector<double> mean({b, g, r});

    return mean;
}

vector<double> feature::getPAR(IplImage *src, int mask)
{
    double perimeter, area, rc, i;
    perimeter = area = i = 0;
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contours = 0;
    cvFindContours(src, storage, &contours, sizeof(CvContour), CV_RETR_LIST);
    if (contours) {
        CvSeq* c = contours;
        for (; c != NULL; c = c->h_next) {
            if (cvContourArea(c) < 1000) continue;
            perimeter += cvArcLength(c);
            area += cvContourArea(c);
//            perimeter = perimeter > cvArcLength(c) ? perimeter : cvArcLength(c);
//            area = area > cvContourArea(c) ? area : cvContourArea(c);
            i++;
            //qDebug("\tmask = %d, i = %d, perimeter = %f, area = %f", mask, i, perimeter, area);
        }
    }
    if (area == 0)
        rc = -1;
    else
        rc = perimeter * perimeter / (4 * 3.14 * area);

    //form feature based on mask
    vector<double> PAR({perimeter, area, rc});

    if (mask == 2) {
        PAR.push_back(i);
    }

    cvReleaseMemStorage(&storage);

    return PAR;
}

