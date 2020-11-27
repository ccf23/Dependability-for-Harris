
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

    cout<<size<<endl;

    // resize output vector
    out.resize(size);

    // read in binary data (vectors are contiguous in memory)
    fin.read((char*)&out[0], size*sizeof(pointData));

    // close fin and exit
    fin.close();
}