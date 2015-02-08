#include "nclseg.h"

nclseg::nclseg()
{
}

IplImage* nclseg::seg(IplImage* img)
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
        water.~Mat();
        cvReleaseImage(&water2);

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

        cvReleaseImage(&waterb);
        cvReleaseImage(&water3backup);
        cvReleaseImage(&water4);
        cvReleaseImage(&luv);
        cvReleaseImage(&mean);


    cvWaitKey(0);
    cvDestroyAllWindows();
    cvSaveImage("wholeCell.png", water3);
    cvSaveImage("nuclei.png", water5);

    cvReleaseImage(&water3);
    cvReleaseImage(&water5);

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
    IplImage* em;
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
    cvSplit(HSV, 0, Is, 0, 0);
    cvSplit(img, Ib, Ig, 0, 0);
    cvThreshold(gray, BW1, 125, 255, CV_THRESH_BINARY_INV);//125
    cvThreshold(gray, BJ, 0, 255, CV_THRESH_OTSU);
    cvThreshold(BJ, BJ, 100, 255, CV_THRESH_BINARY_INV);
    cvThreshold(Ib, BW3, analyse::Otsu(Ib), 255, CV_THRESH_BINARY_INV);
    cvSub(BW3, BW1, Redcell);
    cvSub(BJ, Redcell, BW2);

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
    water.~Mat();
    cvReleaseImage(&water2);

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

    cvReleaseImage(&waterb);
    cvReleaseImage(&water3backup);
    cvReleaseImage(&water4);
    cvReleaseImage(&luv);
    cvReleaseImage(&mean);




    //准备图片，整个细胞bgr， gray，轮廓分析备份，  细胞核bgr， gray，
    IplImage* wholeCell = cvCreateImage(cvGetSize(img), 8, 3);
    IplImage* wholeCellb = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage* wholeCell4Contour= cvCreateImage(cvGetSize(img), 8, 1);
    IplImage* nuclei = cvCreateImage(cvGetSize(img), 8, 3);
    IplImage* nucleib = cvCreateImage(cvGetSize(img), 8, 1);

    cvCopy(water3, wholeCell);
    cvCvtColor(water3, wholeCellb, CV_BGR2GRAY);
    cvCopy(wholeCellb, wholeCell4Contour);
    cvCopy(water5, nuclei);
    cvCvtColor(water5, nucleib, CV_BGR2GRAY);


//    analyse::showImg(img, "img");
//    analyse::showImg(wholeCell, "wholeCell");
//    analyse::showImg(nuclei, "nuclei");
//    cvWaitKey(0);



    cvReleaseImage(&water3);
    cvReleaseImage(&water5);

    //返回值， n * 20 维向量
    QStringList features;

    //对整个细胞gray图进行轮廓分析，找出一个个细胞，然后ROI， 再接着处理
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contours = 0;
    cvFindContours(wholeCell4Contour, storage, &contours, sizeof(CvContour), CV_RETR_LIST);
    if (contours) {
        QStringList eachFeature;
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
            QStringList t1, t2, t22, t3, t4, t5, t6;
            t1 = feature::meanRgb(temp1);
            t3 = feature::meanRgb(temp3);
            t5 = feature::meanRgb(temp5);

            t2 = feature::getPAR(temp2, 2);//4-d
            t4 = feature::getPAR(temp4, 4);//3-d
            t6 << QString::number(t4.at(1).toDouble() - t2.at(1).toDouble()) << QString::number(t2.at(1).toDouble() / t4.at(1).toDouble());//2-d

            t22 = feature::getLBP(temp22);
            //细胞核面积为0，说明ROI设置有错，跳过，特征值不要啦
            if (t2.at(1).toDouble() == 0) {
                //qDebug("empty ROI");
                continue;
            }
//            //如果细胞质面积为0，说明没有细胞质， 应该对BGR分量另外处理

//            if (t6.at(0).toDouble() == 0) {
//                eachFeature << t2.join("'") << t1.join("'") << t4.join("'") << t3.join("'") << "0'0'0" << t6.join("'") <<t22.join("'");
//            }
//            else {
//                eachFeature << t2.join("'") << t1.join("'") << t4.join("'") << t3.join("'") << t5.join("'") << t6.join("'") << t22.join("'");
//            }

/************************************************************************/
//Jan 21 modified, extract xibaozhi feature
            eachFeature << t2.join("'") << t1.join("'") <<t22.join("'");
/**********************************************************************/

            //把当前细胞的特征值放入图片总特征值中
            features << eachFeature.join("'");
            cout << eachFeature.join("'").toLocal8Bit().data() << endl;

            //清空每张图片特征值的list，为下一次做准备
            eachFeature.clear();

//            analyse::showImg(temp1, "temp1");
//            analyse::showImg(temp3, "temp3");
//            if (cvWaitKey(0)== 27) break;

            t1.clear();
            t2.clear();
            t3.clear();
            t4.clear();
            t5.clear();
            t6.clear();

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
