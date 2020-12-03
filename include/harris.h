/*
 *      Author: alexanderb
 */

#ifndef _HARRIS_H_HPP_
#define _HARRIS_H_HPP_

#include <opencv2/opencv.hpp>

#include <opencv2/opencv.hpp>
#include "util.h"
#include "injector.h"
#include "processing.h"
#include "hamming_codes.h"


using namespace std;
using namespace cv;

static unsigned long int total_fi_time = 0;
static unsigned long int get_maxima_fi_time = 0;


#define ASSERTIONS_ON 			  		false
#define ABFT_ON 				      	false
#define THREADS_ON 				    	true
#define HAMMING_ON         	  			false

#define INJECT_FAULTS			    	true

#define DATA_COLLECTION_MODE 			true	// enable this for logging and to disable printing to std::out

#define POSITION_RANGE			  		5		// range for points in processing::process()

static const int THREADS_NUM_FAULTS_TOLERATED_HIGH 	 = 580000;	// Number of faults in image that threading will tolerate
static const int THREADS_NUM_FAULTS_TOLERATED_LOW  = 740;    

#define LOCAL 					      	false	// set to true to allow for visual test on local machine

#if ASSERTIONS_ON

  //iterate through values in matrix

  int iterateFlo(float Flo, float lB, float uB);

#endif

class Harris {
public:
  Harris(Mat img, float k, int filterRange);
	vector<pointData> getMaximaPoints(float percentage, int filterRange, int suppressionRadius);
  	runStats& getStats() {return stats;}

private:
	Mat convertRgbToGrayscale(Mat& img);
	Derivatives computeDerivatives(Mat& greyscaleImg);
	Derivatives applyGaussToDerivatives(Derivatives& dMats, int filterRange);
	Mat computeHarrisResponses(float k, Derivatives& intMats);

#if THREADS_ON
	Mat runParallel_convertRgbToGrayscale(Mat& img);
	Derivatives runParallel_applyToDerivatives(Derivatives& dMats, int filterRange);
	bool withinPixelDiffTolerance(Mat mat1, Mat mat2, bool high_thresh);
#endif

	Mat gaussFilter(Mat& img, int range);


private:
	Mat m_harrisResponses;
  Mat hrCc, hrRc; // harris response column and row checks
  runStats stats;


#if INJECT_FAULTS || HAMMING_ON
	uint32_t uFilterRange;
	uint32_t uSuppressionRadius;
	uint32_t uR;
#endif


#if INJECT_FAULTS
	injector fi;
#endif
};

#endif // _HARRIS_H_HPP_
