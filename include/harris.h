/*
 *      Author: alexanderb
 */


#include <opencv2/opencv.hpp>
#include "util.h"
#include "../include/injector.h"


using namespace std;
using namespace cv;

#define ASSERTIONS_ON 		false
#define ABFT_ON 			false
#define THREADS_ON 			false

#define INJECT_FAULTS		false

#define DATA_COLLECTION_MODE true

#if ASSERTIONS_ON

  typedef struct state { // Each var saved is a checkpoint

    float a11A;//E
    float a12A;//E
    float a21A;//E
    float a22A;//E


  } state_t;

  //iterate through values in matrix

  int iterateFlo(float Flo, float lB, float uB);

#endif

class Harris {
public:
  Harris(Mat img, float k, int filterRange);
	vector<pointData> getMaximaPoints(float percentage, int filterRange, int suppressionRadius);
  #if ASSERTIONS_ON
    state ck;
  #endif
private:
	Mat convertRgbToGrayscale(Mat& img);
	Derivatives computeDerivatives(Mat& greyscaleImg);
	Derivatives applyGaussToDerivatives(Derivatives& dMats, int filterRange);
	Mat computeHarrisResponses(float k, Derivatives& intMats);

#if THREADS_ON
	Mat runParallel_convertRgbToGrayscale(Mat& img);
	Derivatives runParallel_computeDerivatives(Mat& greyscaleImg);
	Derivatives runParallel_applyToDerivatives(Derivatives& dMats, int filterRange);
	Mat runParallel_computeHarrisResponses(float k, Derivatives& mDerivatives);
#endif

	Mat gaussFilter(Mat& img, int range);

private:
	Mat m_harrisResponses;
  	Mat hrCc, hrRc; // harris response column and row checks

#if INJECT_FAULTS
	injector fi;
#endif
};
