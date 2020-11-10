/*
 *      Author: alexanderb
 */

#include <opencv2/opencv.hpp>

#include "util.h"

using namespace std;
using namespace cv;

#define ASSERTIONS_ON false
#define LDPC_ON false
#define CHECKPOINTING_ON true
#define ABFT_ON false

#define DATA_COLLECTION_MODE false

#if CHECKPOINTING_ON

  struct state { // Each var saved is a checkpoint
    Mat original;// A may seem like do not need to save original image because it does not change, but saving it prevents imread
    Mat grey; // B
    Mat derivx;// C
    Mat derivy;  //C
    Mat derivxy; //C
    Mat mderivx;// D
    Mat mderivy;  //D
    Mat mderivxy; //D
    Mat corners; //E

    // cK_A
    // save original
    //
    // ck_B
    // keep ck.prev //orifginal
    // save ck. cu //grey
    //
    // ck_C
    // keep ck.prev //grey
    // save ck. cu //deriv
    //
    //
    //
    // // state check;
    // //
    // // // checkpoint a
    // // save check.original
    // //
    // // //checkpoint b
    // // save check.grey
    //
    // when cK_D
    //   save original
    //   could
    //   greyscalederiv
    //
  };

#endif

class Harris {
public:
    Harris(Mat img, float k, int filterRange, bool gauss);
	vector<pointData> getMaximaPoints(float percentage, int filterRange, int suppressionRadius);
  state ck_A;
  state ck_B;
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

int printmat();
