#include "../include/hammingcode.h"

HammingCode::HammingCode() 
{
    std::cout << "HammingCode::HammingCode() - initializing Hamming_code\n";
    printf("size of int: %d\n", sizeof(int));
    printf("size of float: %d\n", sizeof(float));
    printf("size of double: %d\n", sizeof(double));
}

HammingCode::~HammingCode() { }

static constexpr std::uint32_t correct_bit[32] = 
{
    0x0,
    0x10,       0x8,        0x10000, 	0x4,
    0x4000, 	0x400,      0x200000, 	0x2,
    0x2000, 	0x200,      0x400000, 	0x80,
    0x1000000, 	0x8000000, 	0x80000, 	0x1,
    0x1000, 	0x100,      0x800000, 	0x40,
    0x2000000, 	0x10000000, 0x40000, 	0x20,
    0x4000000, 	0x20000000, 0x20000, 	0x40000000,
    0x8000, 	0x800,      0x100000
};

/**************************** int Hamming ****************************/ 
int32_t HammingCode::encode_H3126(int32_t rawData)
{
    return ((rawData & 0x3ffffff) << 5)
             | (bit_sum_32(rawData & 0x3fff80) << 4)
             | (bit_sum_32(rawData & 0x1c7f878) << 3)
             | (bit_sum_32(rawData & 0x2d9e666) << 2)
             | (bit_sum_32(rawData & 0x36ad555) << 1)
             | bit_sum_32(rawData & 0x3b4b4cb);
}
int32_t HammingCode::encode(int32_t data)
{
    add_parity(encode_H3126(data));

}
int32_t HammingCode::decode(int32_t encodedData)
{
    return ((encodedData >> 6) & 0x3ffffff);
}
bool HammingCode::isCorrectable(int32_t encodedData)
{
    return check_data(encodedData) & 1;
}
bool HammingCode::isCorrect(int32_t encodedData)
{
    return check_data(encodedData) == 0;
}

void HammingCode::correct(int32_t &encodedData)
{
    std::int32_t f = check_data(encodedData);
    if(f & 1) encodedData ^= correct_bit[f >> 1];
}

/**************************** double Hamming ****************************/ 
// double HammingCode::encode(double rawData) { return 0.0;}
// double HammingCode::decode(double encodedData) { return 0.0;}
// bool HammingCode::isCorrect(double encodedData) { return false;}
// bool HammingCode::isCorrectable(double encodedData) { return false;}
// void HammingCode::correct (double &encodedData) {}

/************************ helper functions ***************************/
uint8_t HammingCode::bit_sum_32(int32_t b)
{
    b = ((b & 0xAAAAAAAA) >>  1) + (b & 0x55555555);
    b = ((b & 0xCCCCCCCC) >>  2) + (b & 0x33333333);
    b = ((b & 0xF0F0F0F0) >>  4) + (b & 0x0F0F0F0F);
    b = ((b & 0xFF00FF00) >>  8) + (b & 0x00FF00FF);
    b = ((b & 0xFFFF0000) >> 16) + (b & 0x0000FFFF);
    return b & 1;
}

int32_t HammingCode::check_data(int32_t c)
{
    return (bit_sum_32(c & 0xed2d32c2) << 5)
            | (bit_sum_32(c & 0xdab55544) << 4)
            | (bit_sum_32(c & 0xb6799988) << 3)
            | (bit_sum_32(c & 0x71fe1e10) << 2)
            | (bit_sum_32(c & 0xfffe020) << 1)
            | (bit_sum_32(c & 0xffffffff));
}

int32_t HammingCode::add_parity(int32_t data)
{
    return (data << 1) | bit_sum_32(data);
}