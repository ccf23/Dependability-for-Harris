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
    unsigned long int greyscale;   // greyscale conversion time
    unsigned long int derivatives; // derivative calculation time
    unsigned long int filtering;   // gaussian smoothing time
    unsigned long int response;    // harris response calculation time
    unsigned long int harris;      // total harris calculation time
    unsigned long int features;    // total time to extract features
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
};


#endif // PROCESSING_H