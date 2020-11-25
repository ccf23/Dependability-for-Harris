#ifndef _HAMMING_CODE_HPP_
#define _HAMMING_CODE_HPP_

#include <opencv2/opencv.hpp>

class HammingCode
{
    public:

        HammingCode();
        ~HammingCode();

        void encode(cv::Mat img);
        void decode(cv::Mat img);

    private:

        
};

#endif // _HAMMING_CODE_HPP_