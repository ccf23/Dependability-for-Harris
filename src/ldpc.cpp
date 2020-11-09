#include "../include/ldpc.h"

LDPC::LDPC() :
    _ldpc_code()
{
    
}

LDPC::~LDPC() { }

void LDPC::generateParityMatrix()
{

}

void LDPC::encode(cv::Mat img)
{
    // Convert cv::Mat img to itpp::bvec image
    const itpp::bvec image = mat2bvec(img); 

    _ldpc_code.encode(image);
}

itpp::bvec LDPC::mat2bvec(cv::Mat img)
{
    std::vector<uchar> array(mat.rows*mat.cols*mat.channels());
    if (mat.isContinuous())
        array = mat.data;
    const itpp::bvec image = itpp::to_bvec(array); 
    return image;
}

void LDPC::decode(cv::Mat img)
{
    // Convert cv::Mat img to itpp::bvec image
    const itpp::bvec image; 
    _ldpc_code.decode(image);

}