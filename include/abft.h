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

const int maxErrorLimit = 5000;

// adds checksums to grayscale abft and returns to be used for conversion
Mat doGrayscaleABFT(Mat);

// performs check for greyscale abft
bool grayscaleABFTCheck(Mat&);

// generates checksums for rows and columns of the input matrix
bool abft_addChecksums(Mat, Mat&, Mat&);

// performs verification and correction using matrix and row/column checks
bool abft_check(Mat & , Mat&, Mat&, bool);



#endif