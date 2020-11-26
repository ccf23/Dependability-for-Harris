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

bool abft_addChecksums(Mat, Mat&, Mat&);

bool abft_check(Mat & , Mat&, Mat&);



#endif