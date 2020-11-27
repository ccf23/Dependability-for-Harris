/*
 *      Author: alexanderb
 */

#include <opencv2/opencv.hpp>

#include "util.h"

using namespace std;
using namespace cv;

#define ASSERTIONS_ON false
#define LDPC_ON false
#define CHECKPOINTING_ON false
#define ABFT_ON true

#define DATA_COLLECTION_MODE false

class Harris {
public:
    Harris(Mat img, float k, int filterRange, bool gauss);
	vector<pointData> getMaximaPoints(float percentage, int filterRange, int suppressionRadius);

private:
	Mat convertRgbToGrayscale(Mat& img);
	Derivatives computeDerivatives(Mat& greyscaleImg);	
	Derivatives applyGaussToDerivatives(Derivatives& dMats, int filterRange);
	Mat computeHarrisResponses(float k, Derivatives& intMats);

	Mat gaussFilter(Mat& img, int range);

private:
	Mat m_harrisResponses;
  Mat hrCc, hrRc; // harris response column and row checks
};