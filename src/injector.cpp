/**
 * @file injector.cpp
 * @author Daniel Stumpp
 * @brief fault injector implementation
 * @version 0.1
 * @date 2020-11-16
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "../include/injector.h"
#include <inttypes.h>
#include <time.h>

injector::injector(INJECTOR_MODE_TYPE mode = NONE, double mhp = 0, double rhp = 0)
{
    mem_bit_hit_prob = mhp;
    reg_bit_hit_prob = rhp;
    default_mode = mode;
    injections = 0;
    mem_injections = 0;
    reg_injections = 0;

    std::srand(time(NULL)); // seed rng
}

void injector::enable(void)
{
    enabled = true;
}

void injector::disable(void)
{
    enabled = false;
}

template <typename T> void injector::inject(T &data, INJECTOR_MODE_TYPE mode)
{
    int size = sizeof(T);
    uint8_t *bytes = reinterpret_cast<uint8_t*>(&data);
    
    if (mode == SINGLE_DATA)
    {      
        int bytePos = std::rand() % size; // randomly generate byte pos
        int bitPos = std::rand() % 8;     // randomly generate bit pos

        uint8_t mask = std::pow(2,bitPos); // generate bit mask

        bytes[bytePos] ^= mask; // flip bit in vale
        injections++;
    }
    else if (mode == DOUBLE_DATA)
    {
        int bytePos1 = std::rand() % size; // randomly generate byte pos
        int bitPos1 = std::rand() % 8;     // randomly generate bit pos
        int bytePos2, bitPos2;
        do 
        {
            bytePos2 = std::rand() % size;
            bitPos2 = std::rand() % 8;
        } while (bitPos1 == bitPos2 && bytePos1 == bytePos2);

        uint8_t mask1 = std::pow(2,bitPos1); // generate bit mask
        uint8_t mask2 = std::pow(2,bitPos2);

        bytes[bytePos1] ^= mask1;
        bytes[bytePos2] ^= mask2;
        injections+=2;        
    }
    else if (mode == PROB_DATA)
    {
        unsigned long int oneOutOf = static_cast<unsigned long int>(1.0/bit_hit_prob);
        
        // determine if each bit get's hit based on prob
        for (int by = 0; by < size; ++by)
        {
            for (int bi = 0; bi < 8; bi++)
            {
                unsigned long int safe = std::rand() % oneOutOf;
                if (!safe)
                {
                    bytes[by] ^= static_cast<uint8_t>(std::pow(2,bi));
                    injections++;
                }
            }
        }

    }
}

template <typename T> void injector::inject(T &data)
{
    // call injector with default mode
    inject(data, default_mode);
}

void injector::inject(cv::Mat &data, INJECTOR_MODE_TYPE mode)
{
    int rows = data.rows;
    int cols = data.cols;
    int type = data.type();

    // injection strategy is different for different types
    if (type == CV_32F)
    {
        // select injection type
        if (mode == SINGLE_DATA)
        {
            int rowPos = std::rand() % rows;
            int colPos = std::rand() % cols;

            inject(data.at<float>(rowPos, colPos), SINGLE_DATA);
        }
        else if (mode == DOUBLE_DATA)
        {
            int rowPos1 = std::rand() % rows;
            int colPos1 = std::rand() % cols;
            int rowPos2, colPos2;
            do
            {
                rowPos2 = std::rand() % rows;
                colPos2 = std::rand() % cols;
            } while (colPos1 == colPos2 && rowPos1 == rowPos2);

            inject(data.at<float>(rowPos1, colPos1), SINGLE_DATA);
            inject(data.at<float>(rowPos2, colPos2), SINGLE_DATA);
        }
        else if (mode == PROB_DATA)
        {
            for (int r = 0; r < rows; ++r)
            {
                for (int c = 0; c < cols; ++c)
                {
                    inject(data.at<float>(r,c), PROB_DATA);
                }
            }
        }
    }
    
}

void injector::inject(cv::Mat &data)
{
    // call Mat injector with default mode
    inject(data, default_mode);
}

std::string injector::stats(void)
{
    std::string mode_str = "";
    switch (default_mode)
    {
    case SINGLE_DATA:
        mode_str = "SINGLE_DATA";
        break;
    case DOUBLE_DATA:
        mode_str = "DOUBLE_DATA";
        break;
    case MIXED_DATA:
        mode_str = "MIXED_DATA";
        break;
    case PROB_DATA:
        mode_str = "PROB_DATA";
        break;
    case NONE:
        mode_str = "NONE";
        break;
    default:
        break;
    }
    
    return ("Default Mode: " + mode_str + "\nTotal Injections: " + \
             std::to_string(injections) + "\n");
}

// explicitly defines valid template types
template void injector::inject<uint8_t>(uint8_t&);
template void injector::inject<int8_t>(int8_t&);
template void injector::inject<uint16_t>(uint16_t&);
template void injector::inject<int16_t>(int16_t&);
template void injector::inject<uint32_t>(uint32_t&);
template void injector::inject<int32_t>(int32_t&);
template void injector::inject<char>(char&);
template void injector::inject<long int>(long int&);
template void injector::inject<unsigned long int>(unsigned long int&);
template void injector::inject<float>(float&);
template void injector::inject<double>(double&);
template void injector::inject<long double>(long double&);

template void injector::inject<uint8_t>(uint8_t&, INJECTOR_MODE_TYPE);
template void injector::inject<int8_t>(int8_t&, INJECTOR_MODE_TYPE);
template void injector::inject<uint16_t>(uint16_t&, INJECTOR_MODE_TYPE);
template void injector::inject<int16_t>(int16_t&, INJECTOR_MODE_TYPE);
template void injector::inject<uint32_t>(uint32_t&, INJECTOR_MODE_TYPE);
template void injector::inject<int32_t>(int32_t&, INJECTOR_MODE_TYPE);
template void injector::inject<char>(char&, INJECTOR_MODE_TYPE);
template void injector::inject<long int>(long int&, INJECTOR_MODE_TYPE);
template void injector::inject<unsigned long int>(unsigned long int&, INJECTOR_MODE_TYPE);
template void injector::inject<float>(float&, INJECTOR_MODE_TYPE);
template void injector::inject<double>(double&, INJECTOR_MODE_TYPE);
template void injector::inject<long double>(long double&, INJECTOR_MODE_TYPE);