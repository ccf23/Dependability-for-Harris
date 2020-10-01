/*
 *      Author: alexanderb
 */

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <chrono>
using namespace std::chrono;

#include "../include/harris.h"

using namespace cv;
using namespace std;

Mat m_img;
string filename;

void doHarris() {
    int boxFilterSize = 3;
    int maximaSuppressionDimension = 10;
    bool gauss = true;
    float percentage = 50e-5;
    int markDimension = 5;
    float k = 0.25;
    
    // compute harris
    auto t_before = high_resolution_clock::now();
    Harris harris(m_img, k, boxFilterSize, gauss);
    auto t_after = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(t_after - t_before);
    cout << "Total time to compute Harris: " << duration.count() << " us" << endl;

    // get vector of points wanted
    t_before = high_resolution_clock::now();

    vector<pointData> resPts = harris.getMaximaPoints(percentage, boxFilterSize, maximaSuppressionDimension);
    t_after = high_resolution_clock::now();
    duration = duration_cast<microseconds>(t_after - t_before);
    cout << "Total time to get vector of points: " << duration.count() << " us" << endl;
    // cout << resPts.size() << " Points" << endl;

    t_before = high_resolution_clock::now();
    Mat _img = Util::MarkInImage(m_img, resPts, markDimension);
    t_after = high_resolution_clock::now();
    duration = duration_cast<microseconds>(t_after - t_before);
    cout << "Time to mark image: " << duration.count() << " us" << endl;

    imshow("HarrisCornerDetector", _img);
}

//-----------------------------------------------------------------------------------------------
int main(int argc, char** argv) {
    // read image from file + error handling
    Mat img;

    if (argc == 1) {
        cout << "No image provided! Usage: ./Ex1 [path to image]" << endl << "Using default image: haus.jpg" << endl;

        img = imread("haus.jpg");
    } else {
        filename = argv[1];
        img = imread(argv[1]);
    }

    img.copyTo(m_img);

    auto t_before = high_resolution_clock::now();
    doHarris();
    auto t_after = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(t_after - t_before);
    cout << "Total execution time: " << duration.count() << " us" << endl;
    
    waitKey(0);

    return 0;

}

