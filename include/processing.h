#ifndef PROCESSING_H
#define PROCESSING_H

#include "util.h"

struct featureStats {
	uint missing_features; // number of features in benchmark not found in test
	uint false_features;   // number of features found in test that don't have match in benchmark
	uint bench_features;   // number of features in benchmark
	uint test_features;    // number of features found in test
	uint match_features;   // number of features that match between benchmark and test
};

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