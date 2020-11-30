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
#include <chrono>

typedef std::chrono::_V2::system_clock::time_point injector_time_t;

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
    long double bit_hit_prob;             // probability of a single memory bit being hit
    bool enabled;                    // true when fault injection is enabled
    unsigned long int injections;    // total number of injections performed
    unsigned long int total_time;    // total time (us) used by injector
    injector_time_t last_start;      // last system start time

public:
    // create an injector initialized with the specific default mode
    injector(INJECTOR_MODE_TYPE mode, long double bhp);

    // change bit hit probability
    void setBHP(long double bhp);

    // returns current number of total bit injections performed
    unsigned long int getInjections();

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

    // starts timing injector function
    void tic(void);

    // finish timing injector and add count to total_time
    void toc(void);

    // returns the current time expended by injector in us
    unsigned long int getTime(void);

};

#endif