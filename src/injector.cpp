#include "../include/injector.h"

injector::injector(INJECTOR_MODE_TYPE mode, double mhp = 0, double rhp = 0)
{
    mem_bit_hit_prob = mhp;
    reg_bit_hit_prob = rhp;
    default_mode = mode;
}

void injector::enable(void)
{
    enabled = true;
}

void injector::disable(void)
{
    enabled = false;
}

template <class T> void injector::inject(T &data, INJECTOR_MODE_TYPE mode)
{
    injections++;
    return;
}

template <class T> void injector::inject(T &data)
{
    // call injector with default mode
    inject(data, default_mode);
}

void injector::inject(cv::Mat &data, INJECTOR_MODE_TYPE mode)
{
    injections++;
    return;
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
             std::to_string(injections) + "\nTotal Injections: " + \
             std::to_string(mem_injections) + "Reg Injections: " + \
             std::to_string(reg_injections) + "\n");
}