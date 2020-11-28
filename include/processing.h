#ifndef PROCESSING_H
#define PROCESSING_H

#include "util.h"

typedef struct featureStats {
	uint missing_features; // number of features in benchmark not found in test
	uint false_features;   // number of features found in test that don't have match in benchmark
	uint bench_features;   // number of features in benchmark
	uint test_features;    // number of features found in test
	uint match_features;   // number of features that match between benchmark and test
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
} timingStats;

typedef struct runStats {
    featureStats features;
    timingStats timing;
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
    static void log(runStats stats, std::string filename);
};


#endif // PROCESSING_H