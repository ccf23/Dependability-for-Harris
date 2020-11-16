#ifndef INJECTOR_H
#define INJECTOR_H

#include <opencv2/opencv.hpp>

typedef enum{
    SINGLE_DATA, // single bit error injected for each inject call
    DOUBLE_DATA, // double bit error injected for each inject call
    MIXED_DATA,  // random mix of single and double bit errors
    PROB_DATA,   // injections made based on probability model
    NONE         // injections not performed
} INJECTOR_MODE_TYPE;

class injector
{
private:
    INJECTOR_MODE_TYPE default_mode; // default mode for injections
    double mem_bit_hit_prob; // probability of a single memory bit being hit
    double reg_bit_hit_prob; // probability of a single register bit being hit
    bool enabled; // true when fault injection is enabled
    unsigned int injections;
    unsigned int mem_injections;
    unsigned int reg_injections;

public:
    // create an injector initialized with the specific default mode
    injector(INJECTOR_MODE_TYPE mode, double mhp, double rhp);

    // enable fault injection
    void enable(void);

    // disable fault injection
    void disable(void);

    // injects faults into arbitrary c++ data type with mode specified
    template <typename T> void inject(T &data, INJECTOR_MODE_TYPE mode);

    // injects faults into arbitrary c++ data type with default mode
    template <typename T> void inject(T &data);

    // injects faults into openCV Mat
    void inject(cv::Mat &data, INJECTOR_MODE_TYPE mode);

    // injects faults into openCV Mat with default mode
    void inject(cv::Mat &data);

    // returns injection statistics as a string
    std::string stats(void);

};

//#include "../src/injector.cpp"

#endif