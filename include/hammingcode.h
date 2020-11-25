#ifndef _HAMMING_CODE_HPP_
#define _HAMMING_CODE_HPP_

#include <opencv2/opencv.hpp>

/* adapted from https://github.com/2bbb/ofxHammingCode */

/*
G =
1000 0000 0000 0000 0000 0000 0000 111
0100 0000 0000 0000 0000 0000 0001 011
0010 0000 0000 0000 0000 0000 0001 101
0001 0000 0000 0000 0000 0000 0001 110
0000 1000 0000 0000 0000 0000 0010 011
0000 0100 0000 0000 0000 0000 0010 101
0000 0010 0000 0000 0000 0000 0010 110
0000 0001 0000 0000 0000 0000 0011 001
0000 0000 1000 0000 0000 0000 0011 010
0000 0000 0100 0000 0000 0000 0011 100
0000 0000 0010 0000 0000 0000 0011 111
0000 0000 0001 0000 0000 0000 0011 110
0000 0000 0000 1000 0000 0000 0011 101
0000 0000 0000 0100 0000 0000 0011 011
0000 0000 0000 0010 0000 0000 0011 000
0000 0000 0000 0001 0000 0000 0010 111
0000 0000 0000 0000 1000 0000 0010 100
0000 0000 0000 0000 0100 0000 0010 010
0000 0000 0000 0000 0010 0000 0010 001
0000 0000 0000 0000 0001 0000 0001 111
0000 0000 0000 0000 0000 1000 0001 100
0000 0000 0000 0000 0000 0100 0001 010
0000 0000 0000 0000 0000 0010 0001 001
0000 0000 0000 0000 0000 0001 0000 110
0000 0000 0000 0000 0000 0000 1000 101
0000 0000 0000 0000 0000 0000 0100 011

H =
1110 1101 0010 1101 0011 0010 1100 0010 = 0xed2d32c2
1101 1010 1011 0101 0101 0101 0100 0100 = 0xdab55544
1011 0110 0111 1001 1001 1001 1000 1000 = 0xb6799988
0111 0001 1111 1110 0001 1110 0001 0000 = 0x71fe1e10
0000 1111 1111 1111 1110 0000 0010 0000 = 0xfffe020
1111 1111 1111 1111 1111 1111 1111 1111 = 0xffffffff
*/

class HammingCode
{
    public:

        HammingCode();
        ~HammingCode();
        
        /* Functions for int32_t (int) encoding using (31,26) Hamming */
        int32_t encode_H3126(int32_t rawData);
        int32_t encode(int32_t data);
        int32_t decode(int32_t encodedData);
        bool isCorrectable(int32_t encodedData);
        bool isCorrect(int32_t encodedData);
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

        uint8_t bit_sum_32(int32_t b);
        int32_t check_data(int32_t c);
        int32_t add_parity(int32_t data);
};

#endif // _HAMMING_CODE_HPP_