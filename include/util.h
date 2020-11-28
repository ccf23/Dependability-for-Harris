/*
 *      Author: alexanderb
 */
#ifndef UTIL_H
#define UTIL_H

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

struct featureStats {
	uint missing_features; // number of features in benchmark not found in test
	uint false_features;   // number of features found in test that don't have match in benchmark
	uint bench_features;   // number of features in benchmark
	uint test_features;    // number of features found in test
	uint match_features;   // number of features that match between benchmark and test
};

struct pointData {
    float cornerResponse;

    Point point;
	bool operator==(const pointData &) const;

};

struct by_cornerResponse {
    bool operator()(pointData const &left, pointData const &right) {
        return left.cornerResponse > right.cornerResponse;
    }
};

struct Derivatives {
	Mat Ix;
	Mat Iy;
	Mat Ixy;
};

class Util {
public:
	static void DisplayImage(Mat& img);
	static void DisplayMat(Mat& img);
	static void DisplayPointVector(vector<Point> vp);

	static Mat MarkInImage(Mat& img, vector<pointData> points, int radius);
};

#endif // UTIL_H
