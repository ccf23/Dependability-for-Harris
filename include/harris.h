/*
 *      Author: alexanderb
 */

// Include Guard
#ifndef HARRIS_H
#define HARRIS_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp> //minMaxLoc
#include "util.h"

using namespace std;
using namespace cv;

#define LDPC_ON false
#define ASSERTIONS_ON true
#define ABFT_ON false

#define DATA_COLLECTION_MODE false

#if ASSERTIONS_ON

  typedef struct state { // Each var saved is a checkpoint

    Mat greyA; // B

    Mat derivxA;// C

    Mat derivyA;  //C

    Mat derivxyA; //C

    Mat mderivxA;// D

    Mat mderivyA;  //D

    Mat mderivxyA; //D

    float traceA;

    Mat cornersA; //E

  } state_t;

  //iterate through values in matrix
  int iterateMat(Mat matrix, float lB, float uB);
//  int iterateFlo(float Flo, int lB, int uB);

#endif

class Harris {
public:
  Harris(Mat img, float k, int filterRange, bool gauss);
	vector<pointData> getMaximaPoints(float percentage, int filterRange, int suppressionRadius);
  #if ASSERTIONS_ON
    state ck;
  #endif
private:
	Mat convertRgbToGrayscale(Mat& img);
	Derivatives computeDerivatives(Mat& greyscaleImg);
	Derivatives applyMeanToDerivatives(Derivatives& dMats, int filterRange);
	Derivatives applyGaussToDerivatives(Derivatives& dMats, int filterRange);
	Mat computeHarrisResponses(float k, Derivatives& intMats);

	Mat computeIntegralImg(Mat& img);
	Mat meanFilter(Mat& intImg, int range);
	Mat gaussFilter(Mat& img, int range);

private:
	Mat m_harrisResponses;
};


#endif
