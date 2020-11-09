#ifndef _LDPC_H_
#define _LDPC_H_

#include <opencv2/opencv.hpp>
#include <itpp/comm/ldpc.h>

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

        itpp::LDPC_Code _ldpc_code;

        itpp::bvec mat2bvec(cv::Mat img);
};

#endif // _LDPC_H_