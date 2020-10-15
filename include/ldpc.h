#ifndef _LDPC_H_
#define _LDPC_H_

#include <opencv2/opencv.hpp>

using namespace std;

class LDPC
{
    public:

        LDPC();
        ~LDPC();

        void generateParityMatrix();
        void encode(cv::Mat img);
        void decode(cv::Mat img);

    private:




};

#endif // _LDPC_H_