//
//  Created by ISHII 2bit on 2015/03/29.
//
// https://github.com/2bbb/ofxHammingCode
// 

#pragma once

#include <cstdint>
#include <cstddef>

#define BEGIN_NAMESPACE(name) namespace name {
#define END_NAMESPACE(name) };

#ifndef _UINT32_T
#define _UINT32_T
typedef unsigned int uint32_t;
#endif /* _UINT32_T */

BEGIN_NAMESPACE(ofxHammingCode)

inline static std::uint8_t bit_sum_32(std::uint32_t b) {
    b = ((b & 0xAAAAAAAA) >>  1) + (b & 0x55555555);
    b = ((b & 0xCCCCCCCC) >>  2) + (b & 0x33333333);
    b = ((b & 0xF0F0F0F0) >>  4) + (b & 0x0F0F0F0F);
    b = ((b & 0xFF00FF00) >>  8) + (b & 0x00FF00FF);
    b = ((b & 0xFFFF0000) >> 16) + (b & 0x0000FFFF);
    return b & 1;
}

namespace H3126 {
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
     1110 1101 0010 1101 0011 0010 1100 001
     1101 1010 1011 0101 0101 0101 0100 010
     1011 0110 0111 1001 1001 1001 1000 100
     0111 0001 1111 1110 0001 1110 0001 000
     0000 1111 1111 1111 1110 0000 0010 000
     */

    namespace {
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
        
        inline static std::uint32_t check_data(std::uint32_t c) {
            return (bit_sum_32(c & 0x76969961) << 4)
                | (bit_sum_32(c & 0x6d5aaaa2) << 3)
                | (bit_sum_32(c & 0x5b3cccc4) << 2)
                | (bit_sum_32(c & 0x38ff0f08) << 1)
                | bit_sum_32(c & 0x7fff010);
        }
    };

    inline static std::uint32_t encode(std::uint32_t data) {
        return ((data & 0x3ffffff) << 5)
             | (bit_sum_32(data & 0x3fff80) << 4)
             | (bit_sum_32(data & 0x1c7f878) << 3)
             | (bit_sum_32(data & 0x2d9e666) << 2)
             | (bit_sum_32(data & 0x36ad555) << 1)
             | bit_sum_32(data & 0x3b4b4cb);
    }

    inline static std::uint32_t decode(std::uint32_t h) {
        return ((h >> 5) & 0x3ffffff);
    }
    
    inline static bool isCorrect(std::uint32_t h) {
        return check_data(h) == 0;
    }

    inline static void correct(std::uint32_t &h) {
        h ^= correct_bit[check_data(h)];
    }

    namespace SECDED {
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
        
        namespace {
            static constexpr std::uint32_t correct_bit[32] = {
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
            
            inline static std::uint32_t add_parity(std::uint32_t data) {
                return (data << 1) | bit_sum_32(data);
            }
            
            inline static std::uint32_t check_data(std::uint32_t c) {
                return (bit_sum_32(c & 0xed2d32c2) << 5)
                | (bit_sum_32(c & 0xdab55544) << 4)
                | (bit_sum_32(c & 0xb6799988) << 3)
                | (bit_sum_32(c & 0x71fe1e10) << 2)
                | (bit_sum_32(c & 0xfffe020) << 1)
                | (bit_sum_32(c & 0xffffffff));
            }
        };
        
        /* for uint32_t */
        inline static std::uint32_t encode(std::uint32_t data) {
            return add_parity(H3126::encode(data));
        }
        
        inline static std::uint32_t decode(std::uint32_t h) {
            return ((h >> 6) & 0x3ffffff);
        }

        inline static bool isCorrect(std::uint32_t h) {
            return check_data(h) == 0;
        }
        
        inline static bool isCorrectable(std::uint32_t h) {
            return check_data(h) & 1;
        }
        
        inline static void correct(std::uint32_t &h) {
            std::uint32_t f = check_data(h);
            if(f & 1) h ^= correct_bit[f >> 1];
        }

        /* for 32-bit floats*/
        inline static uint32_t float2int(float f) {
            uint32_t ret;
            std::memcpy(&ret, &f, sizeof(float));
            return ret;
        }

        inline static float int2float(uint32_t f) {
            float ret;
            std::memcpy(&ret, &f, sizeof(float));
            return ret;
        }

        inline static float encodeF(float data) {
            uint32_t as_int = float2int(data);
            as_int = add_parity(H3126::encode(as_int));
            return int2float(as_int);
        }
        
        inline static float decodeF(float h) {
            uint32_t as_int = float2int(h);
            as_int = ((as_int >> 6) & 0x3ffffff);
            return int2float(as_int);
        }

        inline static bool isCorrectF(float h) {
            uint32_t as_int = float2int(h);
            as_int = check_data(as_int) == 0;
            return int2float(as_int);
        }
        
        inline static bool isCorrectableF(float h) {
            uint32_t as_int = float2int(h);
            as_int = check_data(as_int) & 1;
            return int2float(as_int);
        }
        
        inline static void correctF(float &h) {
            uint32_t as_int = float2int(h);
            uint32_t f = check_data(as_int);
            if(f & 1) as_int ^= correct_bit[f >> 1];
            h = int2float(as_int);
        }
    };
};

END_NAMESPACE(ofxHammingCode)