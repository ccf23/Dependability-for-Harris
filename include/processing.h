#ifndef PROCESSING_H
#define PROCESSING_H

#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "util.h"


typedef struct featureStats {
	uint missing_features; // number of features in benchmark not found in test
	uint false_features;   // number of features found in test that don't have match in benchmark
	uint bench_features;   // number of features in benchmark
	uint test_features;    // number of features found in test
	uint match_features;   // number of features that match between benchmark and test

    featureStats():missing_features(0),false_features(0),bench_features(0),test_features(0),match_features(0) {}
} featureStats;

typedef struct timingStats
{
    uint64_t greyscale;   // greyscale conversion time
    uint64_t derivatives; // derivative calculation time
    uint64_t filtering;   // gaussian smoothing time
    uint64_t response;    // harris response calculation time
    uint64_t harris;      // total harris calculation time
    uint64_t features;    // total time to extract features
    uint64_t total;       // total execution time

    timingStats():greyscale(0),derivatives(0),filtering(0),response(0),harris(0),features(0),total(0){}

} timingStats;

typedef struct abftStats
{
    uint grayscaleRuns;      // number of grayscale runs performed
    uint derivativeRuns;     // number of derivative runs performed
    uint gaussLoopResets;    // number of resets triggered in gaussing filtering
    uint responseLoopResets; // number of resets during response processing loop
    uint gDerivativeRuns;    // number of runs for smoothed derivative
    uint correctedErrors;    // number of errors corrected by ABFT

    abftStats():grayscaleRuns(0),derivativeRuns(0),gaussLoopResets(0),responseLoopResets(0),gDerivativeRuns(0),correctedErrors(0){}

} abftStats;

typedef struct runStats {
    featureStats features;
    timingStats timing;
    abftStats abft;
    uint64_t injections;

    runStats():injections(0){}
} runStats;

class processing
{
    public:

    // saves pointData vector to a binary file
    static void saveVector(std::vector<pointData> vect, std::string filename);

    // reads a pointData vector from a binary file
    static bool readVector(std::vector<pointData> &out, std::string filename);

    // process features using benchmark features and test features
    static void process(std::vector<pointData> bench, std::vector<pointData> test, featureStats &stats);

    // logs statistics to output log file
    static void log(runStats stats, std::string filename, bool benchmark);
};


#endif // PROCESSING_H