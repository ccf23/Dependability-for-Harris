#ifndef ABFT_H
#define ABFT_H

#include <opencv2/opencv.hpp>
#include <vector>
using namespace std;
using namespace cv;

typedef enum
{
    NONE,
    GRAYSCALE
} abftMode;

Mat doGrayscaleABFT(Mat);

bool grayscaleABFTCheck(Mat&);

Mat abft_addChecksums(Mat);

bool abft_check(Mat &);

#endif