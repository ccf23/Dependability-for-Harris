#ifndef _HARRIS_H_
#define _HARRIS_H_

/*
 *      Author: alexanderb
 */

#include <opencv2/opencv.hpp>

#include "util.h"
#include "../include/hammingcode.h"

using namespace std;
using namespace cv;

#define ASSERTIONS_ON 		false
#define HAMMING_ON 			true
#define CHECKPOINTING_ON 	false
#define ABFT_ON 			false

#define INJECT_FAULTS		true

class Harris {
public:
    Harris(Mat img, float k, int32_t filterRange, bool gauss);
	std::vector<pointData> getMaximaPoints(float percentage, int32_t filterRange, int32_t suppressionRadius);

private:
	Mat convertRgbToGrayscale(Mat& img);
	Derivatives computeDerivatives(Mat& greyscaleImg);	
	Derivatives applyMeanToDerivatives(Derivatives& dMats, int32_t filterRange);
	Derivatives applyGaussToDerivatives(Derivatives& dMats, int32_t filterRange);
	Mat computeHarrisResponses(float k, Derivatives& intMats);

	Mat computeIntegralImg(Mat& img);
	Mat meanFilter(Mat& intImg, int32_t range);
	Mat gaussFilter(Mat& img, int32_t range);

private:
	Mat m_harrisResponses;
	HammingCode _hamming;
};

#endif // _HARRIS_H_