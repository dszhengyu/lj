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
