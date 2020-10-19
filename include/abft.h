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
/*
struct abftMat
{
    Mat img;
    bool hasRowSum;
    bool hasColSum;

    abftMat(Mat m, abftMode mode);
    //Mat doGrayscaleABFT(Mat);
    Mat encode(Mat, abftMode);
};*/

Mat doGrayscaleABFT(Mat);

bool grayscaleABFTCheck(Mat&);

#endif