
#include <fstream>
#include "../include/processing.h"

void processing::saveVector(std::vector<pointData> vect, std::string filename)
{
    // destination filename
    std::string fn = filename + ".dat";

    // ofstream to write data as binary
    std::ofstream fout(fn, ios::binary);

    // get size of vector
    uint64_t size = vect.size();
    
    // write vector size to fout
    fout.write((const char*)&size, sizeof(size));

    // write vector data to fout
    fout.write((const char*)&vect[0], size*sizeof(pointData));

    // close fout and exit
    fout.close();
}

bool processing::readVector(std::vector<pointData> &out, std::string filename)
{
    // data filename
    std::string fn = filename + ".dat";

    // ifstream to read binary data in
    std::ifstream fin(fn, ios::binary);

    // size value to get length of binary data
    uint64_t size;

    // read in size of data
    fin.read((char*)&size, sizeof(uint64_t));

    // resize output vector
    out.resize(size);

    // read in binary data (vectors are contiguous in memory)
    fin.read((char*)&out[0], size*sizeof(pointData));

    // close fin and exit
    fin.close();

    return true;
}

void processing::process(std::vector<pointData> bench, std::vector<pointData> test, featureStats &stats)
{
 
    // extract number of features detected from both vectors
    stats.bench_features = bench.size();
    stats.test_features  = test.size();

    // set window range
    uint range = 5;

    // initialize other stats values
    stats.false_features = 0;
    stats.missing_features = 0;
    stats.match_features = 0;

    for (uint i = 0; i < stats.bench_features; ++i)
    {
        // exctract current point
        Point bpt = bench.at(i).point;

        // flag for found match
        bool match = false;

        // search for match in test vector
        for (uint j = 0; j < test.size(); ++j)
        {
            // extract test point
            Point tpt = test.at(j).point;

            // check if test point is within range of bench point
            if (std::abs(tpt.x - bpt.x) <= range && std::abs(tpt.y - bpt.y) <= range)
            {
                test.erase(test.begin() + j); // erase point in test
                stats.match_features++;       // increment number of matches
                match = true;                 
                break;
            }
        }

        // if no match then it is a missing feature
        if (!match)
        {
            stats.missing_features++;
        }
    }

    // get number of false features as remaining points in test vector
    stats.false_features = test.size();

}