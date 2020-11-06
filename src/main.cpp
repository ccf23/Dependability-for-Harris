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

// uncomment to allow for visual test on local machine
//#define LOCAL

Mat m_img;
string filename;

void doHarris() {
    int boxFilterSize = 3;
    int maximaSuppressionDimension = 10;
    bool gauss = true;
    float percentage = 50e-5;
    int markDimension = 5;
    float k = 0.25;

    // Temporary: Pattern to follow for enabling/disabling FT:
    #if ASSERTIONS_ON
        cout <<\t<<"Hello world";

    #endif


    // compute harris
    auto t_before = high_resolution_clock::now();
    Harris harris(m_img, k, boxFilterSize, gauss);
    auto t_after = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(t_after - t_before);
    #if DATA_COLLECTION_MODE
        cout << duration.count()/1000  << ",";
    #else
        cout << "Total time to compute Harris: " << duration.count()/1000 << " ms" << endl;
    #endif

    // get vector of points wanted
    t_before = high_resolution_clock::now();

    vector<pointData> resPts = harris.getMaximaPoints(percentage, boxFilterSize, maximaSuppressionDimension);
    t_after = high_resolution_clock::now();
    duration = duration_cast<microseconds>(t_after - t_before);
    #if DATA_COLLECTION_MODE
        cout << duration.count()/1000 << ",";
        cout << resPts.size() << ",";
    #else
        cout << "Total time to get vector of points: " << duration.count()/1000 << " ms" << endl;
        cout << "Features Detected: "<<resPts.size()<<endl;
    #endif

    #ifdef LOCAL
    t_before = high_resolution_clock::now();
    Mat _img = Util::MarkInImage(m_img, resPts, markDimension);
    t_after = high_resolution_clock::now();
    duration = duration_cast<microseconds>(t_after - t_before);
    #if DATA_COLLECTION_MODE
        cout << duration.count()/1000  << ",";
    #else
        cout << "Time to mark image: " << duration.count()/1000 << " ms" << endl;
    #endif
    imshow("HarrisCornerDetector", _img);
    #endif
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


    //first Checkpoint
    #if CHECKPOINTING_ON
    // call struct to see if it is working
    //   struct state checkpoints = {img};
    //   FILE* f = fopen("checkpoints","wb+");
    //   fwrite(&checkpoints, sizeof(struct state), 1, f);// makes one binary per var using sizeof() // doesnt imread come from memory?
      state ck_A;
      ck_A.original = img.clone();

      //if error, go to

    #endif

    img.copyTo(m_img);

    auto t_before = high_resolution_clock::now();
    doHarris();
    auto t_after = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(t_after - t_before);

    #if DATA_COLLECTION_MODE
        cout << duration.count()/1000  << "\n" << endl;
    #else
        cout << "Total execution time: " << duration.count()/1000 << " ms" << endl;
    #endif

    #ifdef LOCAL
        waitKey(0);
    #endif

    return 0;

    //maybe clear all vars in state struct to have a clean slate for next image?

}
