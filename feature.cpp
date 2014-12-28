#include "feature.h"

feature::feature()
{
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

void feature::LBP(IplImage *src, IplImage *dst)
{
    int width = src->width;
    int height = src->height;
    int i, j;
    unsigned char table[256];
    formTable(table);

    for (i = 1; i < height - 1; i++)
        for (j = 0; j < width - 1; j++) {
            unsigned char center = CV_IMAGE_ELEM( src, unsigned char, i, j);
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
        }
}

QStringList feature::selectFeature(IplImage *src1, IplImage *src2)//src1 is wholecell, src2 is nuclei
{
//准备图片，整个细胞bgr， gray，轮廓分析备份，  细胞核bgr， gray，
    IplImage* wholeCell = cvCreateImage(cvGetSize(src1), 8, 3);
    IplImage* wholeCellb = cvCreateImage(cvGetSize(src1), 8, 1);
    IplImage* wholeCell4Contour= cvCreateImage(cvGetSize(src1), 8, 1);
    IplImage* nuclei = cvCreateImage(cvGetSize(src1), 8, 3);
    IplImage* nucleib = cvCreateImage(cvGetSize(src2), 8, 1);

    cvCopy(src1, wholeCell);
    cvCvtColor(src1, wholeCellb, CV_BGR2GRAY);
    cvCopy(wholeCellb, wholeCell4Contour);
    cvCopy(src2, nuclei);
    cvCvtColor(src2, nucleib, CV_BGR2GRAY);

//返回值， n * 20 维向量
    QStringList features;

//对整个细胞gray图进行轮廓分析，找出一个个细胞，然后ROI， 再接着处理
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contours = 0;
    cvFindContours(wholeCell4Contour, storage, &contours, sizeof(CvContour), CV_RETR_LIST);
    if (contours) {
        QStringList eachFeature;
        CvSeq* c = contours;
        int i = 0;
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
            IplImage* temp3 = cvCreateImage(cvGetSize(wholeCell), 8, 3);//整个细胞bgr
            IplImage* temp4 = cvCreateImage(cvGetSize(wholeCellb), 8, 1);//整个细胞gray
            IplImage* temp5 = cvCreateImage(cvGetSize(wholeCell), 8, 3);//细胞质bgr
//            IplImage* temp6 = cvCreateImage(cvGetSize(wholeCellb), 8, 1);//细胞质gray
            cvCopy(nuclei, temp1);
            cvCopy(nucleib, temp2);
            cvCopy(wholeCell, temp3);
            cvCopy(wholeCellb, temp4);
            //细胞质图片
            cvSub(temp3, temp1, temp5);
//            cvSub(temp4, temp2, temp6);
            //取消ROI设置
            cvResetImageROI(wholeCell);
            cvResetImageROI(wholeCellb);
            cvResetImageROI(nuclei);
            cvResetImageROI(nucleib);

            //analyse::showImg(temp1, "temp1");
            //analyse::showImg(temp3, "temp3");
            //analyse::showImg(temp5, "temp5");

            //开始提取特征， 用tempX图片
            QStringList t1, t2, t3, t4, t5, t6;
            t1 = feature::meanRgb(temp1);
            t3 = feature::meanRgb(temp3);
            t5 = feature::meanRgb(temp5);

            t2 = feature::getPAR(temp2, 2);//4-d
            t4 = feature::getPAR(temp4, 4);//3-d
            t6 << QString::number(t4.at(1).toDouble() - t2.at(1).toDouble()) << QString::number(t2.at(1).toDouble() / t4.at(1).toDouble());//2-d

            //细胞核面积为0，说明ROI设置有错，跳过，特征值不要啦
            if (t2.at(1).toDouble() == 0) {
                //qDebug("empty ROI");
                continue;
            }
            //如果细胞质面积为0，说明没有细胞质， 应该对BGR分量另外处理

            if (t6.at(0).toDouble() == 0) {
                eachFeature << t2.join("'") << t1.join("'") << t4.join("'") << t3.join("'") << "0'0'0" << t6.join("'");
            }
            else {
                eachFeature << t2.join("'") << t1.join("'") << t4.join("'") << t3.join("'") << t5.join("'") << t6.join("'");
            }

            //把当前细胞的特征值放入图片总特征值中
            features << eachFeature.join("'");
            //qDebug("%s\n", eachFeature.join("'").toLocal8Bit().data());

            //清空每张图片特征值的list，为下一次做准备
            eachFeature.clear();
            //if (cvWaitKey(0)== 27) break;
        }
    }
    return features;
}

QStringList feature::meanRgb(IplImage *src)
{
    double b, g, r;
    QStringList mean;
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
    mean << QString::number(b) << QString::number(g) << QString::number(r);

    //qDebug("%s", mean.join("'").toLocal8Bit().data());
    return mean;
}

QStringList feature::getPAR(IplImage *src, int mask)
{
    double perimeter, area, rc, i;
    QStringList PAR;
    perimeter = area = i = 0;
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contours = 0;
    cvFindContours(src, storage, &contours, sizeof(CvContour), CV_RETR_LIST);
    if (contours) {
        CvSeq* c = contours;
        for (; c != NULL; c = c->h_next) {
            if (cvContourArea(c) < 1000) continue;
            perimeter = perimeter > cvArcLength(c) ? perimeter : cvArcLength(c);
            area = area > cvContourArea(c) ? area : cvContourArea(c);
            i++;
            //qDebug("\tmask = %d, i = %d, perimeter = %f, area = %f", mask, i, perimeter, area);
        }
    }
    if (area == 0)
        rc = -1;
    else
        rc = perimeter * perimeter / (4 * 3.14 * area);

    //form feature based on mask
    PAR << QString::number(perimeter) << QString::number(area) << QString::number(rc);
    if (mask == 2) {
        PAR << QString::number(i);
    }

    //qDebug("%s", PAR.join("'").toLocal8Bit().data());
    return PAR;
}
