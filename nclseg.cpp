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
    IplImage* BW2IE = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    IplImage* water = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
    //IplImage* temp = cvCreateImage(cvGetSize(BW2IE),IPL_DEPTH_32S,1);
    cvCopy(img, water);

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

    //the T should be chosen more adaptive
    cvThreshold(IE, IE, 220, 255, CV_THRESH_BINARY);

    //cvErode(IE, IE, cvCreateStructuringElementEx(3, 3, 2, 2, CV_SHAPE_RECT), 4);
     //cvMorphologyEx(IE, IE, 0, cvCreateStructuringElementEx(3, 3, 2, 2, CV_SHAPE_RECT), CV_MOP_OPEN, 2);
    // cvDilate(IE, IE, cvCreateStructuringElementEx(2, 2, 1, 1, CV_SHAPE_RECT), 5);
     //cvMorphologyEx(IE, IE, 0, cvCreateStructuringElementEx(3, 3, 2, 2, CV_SHAPE_RECT), CV_MOP_OPEN, 2);

    cvMorphologyEx(IE, IE, 0, cvCreateStructuringElementEx(3, 3, 2, 2, CV_SHAPE_RECT), CV_MOP_OPEN, 3);
    cvMorphologyEx(IE, IE, 0, cvCreateStructuringElementEx(3, 3, 2, 2, CV_SHAPE_RECT), CV_MOP_CLOSE, 10);
    analyse::fillHole(IE);
    cvErode(IE, IE, cvCreateStructuringElementEx(3, 3, 2, 2, CV_SHAPE_RECT), 3);
    analyse::showImg(IE, "IE");
    analyse::showImg(analyse::analyseCoutours(IE), "contours");


    /*
    cvAnd(BW2, IE, BW2IE);


    //cvScale(BW2IE, temp, 65535/255);
   // analyse::showImg(temp, "ex-temp");
    //cvWatershed(water, temp);
   // analyse::showImg(temp, "temp");

    //analyse::showImg(watermelon(water, BW2IE), "watershed");

*/


    analyse::showImg(img, "img");

    //analyse::showImg(BW2IE, "BW2IE");
    cvWaitKey(0);
    cvDestroyAllWindows();

    return IE;
}










/*
void nclseg::cvt32to8(IplImage *src, IplImage *dst)
{
    int height= src->height;
    int width = src->width;

    for( int i = 0; i < src->height; i++ )
        for( int j = 0; j < src->width; j++ )
        {
             int idx = CV_IMAGE_ELEM(src, int, i, j );
             unsigned char* dst = &CV_IMAGE_ELEM(dst, unsigned char, i, j );
             *dst = (unsigned char)idx/65535*255;

         }


}
*/

IplImage* nclseg::watermelon(IplImage *water, IplImage *BW2IE)
{
    IplImage* markers = cvCreateImage(cvGetSize(water), IPL_DEPTH_32S, 1);
    IplImage* watershed = cvCreateImage(cvGetSize(water), IPL_DEPTH_8U, 3);
    cvCopy(water, watershed);
    CvMemStorage* storage = cvCreateMemStorage(0);
    cvZero(watershed);
    cvZero(markers);
    CvSeq* contours = 0;
    CvMat* color_tab = 0;
    CvRNG rng = cvRNG(-1);
    int i, j, comp_count = 0;
    cvClearMemStorage(storage);

    cvFindContours( BW2IE, storage, &contours, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
    for( ; contours != 0; contours = contours->h_next, comp_count++ )
        cvDrawContours(markers, contours, cvScalarAll(comp_count+1), cvScalarAll(comp_count+1), -1, -1, 8, cvPoint(0,0) );

    color_tab = cvCreateMat( 1, comp_count, CV_8UC3 );
    for( i = 0; i < comp_count; i++ )
    {
        uchar* ptr = color_tab->data.ptr + i*3;
        ptr[0] = (uchar)(cvRandInt(&rng)%180 + 50);
        ptr[1] = (uchar)(cvRandInt(&rng)%180 + 50);
        ptr[2] = (uchar)(cvRandInt(&rng)%180 + 50);
     }

     cvWatershed(water, markers );
     //cvSave("markers.xml",markers);

     for( i = 0; i < markers->height; i++ )
        for( j = 0; j < markers->width; j++ )
        {
            int idx = CV_IMAGE_ELEM( markers, int, i, j );
            uchar* dst = &CV_IMAGE_ELEM( watershed, uchar, i, j*3 );
            if( idx == -1 )
                 dst[0] = dst[1] = dst[2] = (uchar)255;
            else if( idx <= 0 || idx > comp_count )
                 dst[0] = dst[1] = dst[2] = (uchar)0;
            else {
                 uchar* ptr = color_tab->data.ptr + (idx-1)*3;
                 dst[0] = ptr[0]; dst[1] = ptr[1]; dst[2] = ptr[2];
                 }
         }
     //cvAddWeighted( watershed, 0.5, img_gray, 0.5, 0, watershed );

     return watershed;

}

