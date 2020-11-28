/*
 *      Author: alexanderb
 */

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <fstream>
using namespace std::chrono;

#include "../include/harris.h"
#include "../include/processing.h"

using namespace cv;
using namespace std;

Mat m_img;
string filename;

void doHarris(std::string filename, bool benchmark) {
    auto harris_before = high_resolution_clock::now();
    int boxFilterSize = 3;
    int maximaSuppressionDimension = 15;
    float percentage = 50e-5;
    int markDimension = 5;
    float k = 0.05;

    // compute harris
    auto t_before = high_resolution_clock::now();
    Harris harris(m_img, k, boxFilterSize);
    auto t_after = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(t_after - t_before);

    // get stats struct from harris
    runStats stats = harris.getStats();

    #if DATA_COLLECTION_MODE
        stats.timing.harris = duration.count();
    #else
        cout << "Total time to compute Harris: " << duration.count()/1000 << " ms" << endl;
    #endif

    // get vector of points wanted
    t_before = high_resolution_clock::now();

    vector<pointData> resPts = harris.getMaximaPoints(percentage, boxFilterSize, maximaSuppressionDimension);
    t_after = high_resolution_clock::now();
    duration = duration_cast<microseconds>(t_after - t_before);
    #if DATA_COLLECTION_MODE
        stats.timing.features = duration.count();
    #else
        cout << "Total time to get vector of points: " << duration.count()/1000 << " ms" << endl;
        cout << "Features Detected: "<<resPts.size()<<endl;
    #endif

    if (benchmark)
    {
        // save benchmark data to file
        processing::saveVector(resPts, filename);

        // zero out feature stats
        stats.features.bench_features = resPts.size();
        stats.features.false_features = 0;
        stats.features.match_features = 0;
        stats.features.missing_features = 0;
        stats.features.test_features = 0;
    }
    else
    {
        ifstream f(filename + ".dat");
        assert(f.good());
        std::vector<pointData> bench;
        processing::readVector(bench, filename);

        // process results
        featureStats feat_stats;
        processing::process(bench, resPts, feat_stats);

        stats.features = feat_stats;
    }

    #if LOCAL
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

    t_after = high_resolution_clock::now();
    duration = duration_cast<microseconds>(t_after - harris_before);

    #if DATA_COLLECTION_MODE
        stats.timing.total = duration.count();
    #else
        cout << "Total execution time: " << duration.count()/1000 << " ms" << endl;
    #endif

    #if DATA_COLLECTION_MODE
        // log run statistics
        processing::log(stats, filename, benchmark);
    #endif
}

//-----------------------------------------------------------------------------------------------
int main(int argc, char** argv) {
    // read image from file + error handling
    Mat img;
    bool benchmark = false;

    if (argc == 1) {
        cout << "No image provided! Usage: ./Ex1 [path to image]" << endl << "Using default image: haus.jpg" << endl;

        img = imread("haus.jpg");
    }
    else if (argc == 3)
    {
        filename = argv[1];
        img = imread(argv[1]);

        if (std::string(argv[2]) == "benchmark")
        {
            benchmark = true;
        }
    }
    else
    {
        filename = argv[1];
        img = imread(argv[1]);
    }

    img.copyTo(m_img);

    doHarris(std::string(argv[1]),benchmark);

    #if LOCAL
        waitKey(0);
    #endif

    return 0;
}
