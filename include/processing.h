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
};


#endif // PROCESSING_H