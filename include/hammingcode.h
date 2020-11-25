#ifndef _HAMMING_CODE_HPP_
#define _HAMMING_CODE_HPP_

#include <opencv2/opencv.hpp>

class HammingCode
{
    public:

        HammingCode();
        ~HammingCode();

        /* Functions for int32_t (int) encoding using (31,26) Hamming */
        int32_t encode(int32_t rawData);
        int32_t decode(int32_t encodedData);
        bool isCorrect(int32_t encodedData);
        bool isCorrectable(int32_t encodedData);
        void correct (int32_t &encodedData);

        // TODO : Change all floats (32 bits) in Harris to doubles (64 bits) so there's 
        // room for 6 extra bits of parity

        /* Functions for double encoding using (32,26) Hamming */
        // static double encode(double rawData);
        // static double decode(double encodedData);
        // static bool isCorrect(double encodedData);
        // static bool isCorrectable(double encodedData);
        // static void correct (double &encodedData);

    private:

        uint8_t bit_sum_32_int(int32_t b);

        static constexpr std::uint32_t correct_bit[32] = {
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
        
};

#endif // _HAMMING_CODE_HPP_