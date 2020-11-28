#ifndef PROCESSING_H
#define PROCESSING_H

#include "util.h"

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