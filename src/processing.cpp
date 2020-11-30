
#include "../include/processing.h"
#include "../include/harris.h"

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

            // check if test point is within POSITION_RANGE of bench point
            if (std::abs(tpt.x - bpt.x) <= POSITION_RANGE && std::abs(tpt.y - bpt.y) <= POSITION_RANGE)
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

void processing::log(runStats stats, std::string filename, bool benchmark)
{
    std::string fn = filename + "_log.csv";

    // open log file to append
    std::ofstream fout(fn.c_str(), ios::out | ios::app);

    if (!fout.is_open())
    {
        cout << "processing::log() - error opening file " << fn << ", exiting\n";
        exit(0);
    }

    // write out stats to file stream in csv format
    fout << stats.timing.greyscale << ","   \
         << stats.timing.derivatives << "," \
         << stats.timing.filtering << ","   \
         << stats.timing.response << ","    \
         << stats.timing.harris << ","      \
         << stats.timing.features << ","    \
         << stats.timing.total << ","       \
         << stats.features.bench_features << ","   \
         << stats.features.test_features << ","    \
         << stats.features.match_features << ","   \
         << stats.features.missing_features << "," \
         << stats.features.false_features << ","   \
         << boolalpha << benchmark <<"," \
         << boolalpha << ABFT_ON << "," \
         << boolalpha << ASSERTIONS_ON << "," \
         << boolalpha << THREADS_ON << "," \
         << boolalpha << HAMMING_ON << "," \
         << boolalpha << INJECT_FAULTS << "," \
         << boolalpha << DATA_COLLECTION_MODE << "," \
         << POSITION_RANGE << "," \
         << THREADS_NUM_FAULTS_TOLERATED << "," \
         << boolalpha << LOCAL << "," \
         << stats.abft.grayscaleRuns << "," \
         << stats.abft.derivativeRuns << "," \
         << stats.abft.gaussLoopResets << ","\
         << stats.abft.responseLoopResets << ","\
         << stats.abft.gDerivativeRuns << ","\
         << stats.abft.correctedErrors << "," <<endl;
    
    fout.close();   
}