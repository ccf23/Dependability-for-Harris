#include "../include/hammingcode.h"

HammingCode::HammingCode() { }

HammingCode::~HammingCode() { }

static constexpr std::uint32_t correct_bit[32] = 
{
    0x1,
    0x20,       0x10,       0x20000,    0x8,
    0x8000,     0x800,      0x400000,   0x4,
    0x4000,     0x400,      0x800000,   0x100,
    0x2000000,  0x10000000, 0x100000,   0x2,
    0x2000,     0x200,      0x1000000,  0x80,
    0x4000000,  0x20000000, 0x80000,    0x40,
    0x8000000,  0x40000000, 0x40000,    0x80000000,
    0x10000,    0x1000,     0x200000
};

/**************************** int Hamming ****************************/ 
uint32_t HammingCode::encode_H3126(uint32_t rawData)
{
    return ((rawData & 0x3ffffff) << 5)
        | (bit_sum_32(rawData & 0x3fff80) << 4)
        | (bit_sum_32(rawData & 0x1c7f878) << 3)
        | (bit_sum_32(rawData & 0x2d9e666) << 2)
        | (bit_sum_32(rawData & 0x36ad555) << 1)
        | bit_sum_32(rawData & 0x3b4b4cb);
}
uint32_t HammingCode::encode(uint32_t data)
{
    add_parity(encode_H3126(data));
}

uint32_t HammingCode::decode(uint32_t encodedData)
{
    return ((encodedData >> 6) & 0x3ffffff);
}

bool HammingCode::isCorrectable(uint32_t encodedData)
{
    return check_data(encodedData) & 1;
}

bool HammingCode::isCorrect(uint32_t encodedData)
{
    return check_data(encodedData) == 0;
}

void HammingCode::correct(uint32_t &encodedData)
{
    uint32_t f = check_data(encodedData);
    if (f & 1) encodedData ^= correct_bit[f >> 1];
}

/**************************** float Hamming ****************************/ 
float HammingCode::encode_H3126(float rawData)
{
    uint32_t as_int = float2int(rawData);
    as_int = encode_H3126(as_int);
    return int2float(as_int);
}

float HammingCode::encode(float data)
{
    uint32_t as_int = float2int(data);
    add_parity(encode_H3126(data));
    return int2float(as_int);
}

float HammingCode::decode(float encodedData)
{
    uint32_t as_int = float2int(encodedData);
    as_int = ((as_int >> 6) & 0x3ffffff);
    return int2float(as_int);
}

bool HammingCode::isCorrectable(float encodedData)
{
    uint32_t as_int = float2int(encodedData);
    as_int = check_data(as_int) & 1;
    return int2float(as_int);
}

bool HammingCode::isCorrect(float encodedData)
{
    uint32_t as_int = float2int(encodedData);
    as_int = check_data(as_int) == 0;
    return int2float(as_int);
}

void HammingCode::correct(float &encodedData)
{
    uint32_t as_int = float2int(encodedData);
    uint32_t f = check_data(as_int);
    if (f & 1) as_int ^= correct_bit[f >> 1];
    encodedData = int2float(as_int);
}


/************************ helper functions ***************************/
uint32_t HammingCode::float2int(float f)
{
    static_assert(sizeof(float) == sizeof f, "`float` has a weird size.");
    uint32_t ret;
    std::memcpy(&ret, &f, sizeof(float));
    return ret;
}

float HammingCode::int2float(uint32_t f)
{
    static_assert(sizeof(float) == sizeof f, "`float` has a weird size.");
    float ret;
    std::memcpy(&ret, &f, sizeof(float));
    return ret;
}

uint8_t HammingCode::bit_sum_32(uint32_t b)
{
    b = ((b & 0xAAAAAAAA) >>  1) + (b & 0x55555555);
    b = ((b & 0xCCCCCCCC) >>  2) + (b & 0x33333333);
    b = ((b & 0xF0F0F0F0) >>  4) + (b & 0x0F0F0F0F);
    b = ((b & 0xFF00FF00) >>  8) + (b & 0x00FF00FF);
    b = ((b & 0xFFFF0000) >> 16) + (b & 0x0000FFFF);
    return b & 1;
}

uint32_t HammingCode::check_data(uint32_t c)
{
    return (bit_sum_32(c & 0xed2d32c2) << 5)
            | (bit_sum_32(c & 0xdab55544) << 4)
            | (bit_sum_32(c & 0xb6799988) << 3)
            | (bit_sum_32(c & 0x71fe1e10) << 2)
            | (bit_sum_32(c & 0xfffe020) << 1)
            | (bit_sum_32(c & 0xffffffff));
}

uint32_t HammingCode::add_parity(uint32_t data)
{
    return (data << 1) | bit_sum_32(data);
}