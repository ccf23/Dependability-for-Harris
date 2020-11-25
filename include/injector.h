/**
 * @file injector.h
 * @author Daniel Stumpp
 * @brief fault injector for arbitary c++ types and openCV Mat types
 * @version 0.1
 * @date 2020-11-16
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef INJECTOR_H
#define INJECTOR_H

#include <opencv2/opencv.hpp>

typedef enum{
    SINGLE_DATA, // single bit error injected for each inject call
    DOUBLE_DATA, // double bit error injected for each inject call
    PROB_DATA,   // injections made based on probability model
    NONE         // injections not performed
} INJECTOR_MODE_TYPE;

class injector
{
private:
    INJECTOR_MODE_TYPE default_mode; // default mode for injections
    double bit_hit_prob; // probability of a single memory bit being hit
    bool enabled; // true when fault injection is enabled
    unsigned int injections; // total number of injections performed

public:
    // create an injector initialized with the specific default mode
    injector(INJECTOR_MODE_TYPE mode, double bhp);

    // enable fault injection
    void enable(void);

    // disable fault injection
    void disable(void);

    // change bit hit probability
    void setBHP(double bhp);

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

#endif