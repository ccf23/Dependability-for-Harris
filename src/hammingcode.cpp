#include "../include/hammingcode.h"

HammingCode::HammingCode() 
{
    std::cout << "HammingCode::HammingCode() - initializing Hamming_code\n";
    printf("size of int: %d\n", sizeof(int));
    printf("size of float: %d\n", sizeof(float));
    printf("size of double: %d\n", sizeof(double));
}

HammingCode::~HammingCode() { }

/**************************** int Hamming ****************************/ 
int32_t HammingCode::encode(int32_t rawData)
{
    return 0;
}
int32_t HammingCode::decode(int32_t encodedData)
{
    return 0;
}
bool HammingCode::isCorrect(int32_t encodedData)
{
    return false;
}
bool HammingCode::isCorrectable(int32_t encodedData)
{
    return false;
}
void HammingCode::correct(int32_t &encodedData)
{

}

/**************************** double Hamming ****************************/ 
// double HammingCode::encode(double rawData) { return 0.0;}
// double HammingCode::decode(double encodedData) { return 0.0;}
// bool HammingCode::isCorrect(double encodedData) { return false;}
// bool HammingCode::isCorrectable(double encodedData) { return false;}
// void HammingCode::correct (double &encodedData) {}

/************************ helper functions ***************************/
uint8_t HammingCode::bit_sum_32_int(int32_t b)
{
    b = ((b & 0xAAAAAAAA) >>  1) + (b & 0x55555555);
    b = ((b & 0xCCCCCCCC) >>  2) + (b & 0x33333333);
    b = ((b & 0xF0F0F0F0) >>  4) + (b & 0x0F0F0F0F);
    b = ((b & 0xFF00FF00) >>  8) + (b & 0x00FF00FF);
    b = ((b & 0xFFFF0000) >> 16) + (b & 0x0000FFFF);
    return b & 1;
}