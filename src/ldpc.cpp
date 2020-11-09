#include "../include/ldpc.h"

LDPC::LDPC() 
{
    std::cout << "LDPC::LDPC() - initializing ldpc_code\n";
    _H.generate(500, 3, 6, "rand", "500 10");
    _H.display_stats();
    // _ldpc_code(&_H);
    // _ldpc_code.save_code("random_3_6_code.it");
}

LDPC::~LDPC() { }

void LDPC::encode(cv::Mat img)
{
    std::cout << "LDPC::encode() - entered\n";
    // Convert cv::Mat img to itpp::bvec image
    const itpp::bvec image = mat2bvec(img); 

    std::cout << "LDPC::encode() - calling LDPC_Code::encode()\n";
    _ldpc_code.encode(image);
}

void LDPC::decode(cv::Mat img)
{
    std::cout << "LDPC::decode() - entered\n";

    // Convert cv::Mat img to itpp::bvec image
    const itpp::bvec image; 
    _ldpc_code.decode(image);
}

itpp::bvec LDPC::mat2bvec(cv::Mat img)
{
    std::cout << "LDPC::mat2bvec() - entered\n";

    cv::Size size = img.size();
    int total = size.width * size.height * img.channels();
    // std::cout << "img size = " << total << std::endl;
    std::vector<uchar> data(img.ptr(), img.ptr() + total);
    std::string s(data.begin(), data.end());                   
    // std::cout << "String size = " << s.length() << std::endl;
    itpp::bvec vec;
    vec.set_size((total*8)-7);
    stringToVector(s, vec);
    return vec;
}

void LDPC::stringToVector(const std::string & src, itpp::bvec & dst)
{
    uint32_t dstSizeBits = dst.size();
    uint32_t dstSizeBytes = (dstSizeBits + 7) / 8;

    // Check input size
    if(src.size() != dstSizeBytes) {
        std::cout << "src.size() = " << src.size() << " dstSizeBytes = " << dstSizeBytes << "\n";
        throw(std::runtime_error("String size incompatible with destination's size"));
    }

    // convert the string to a bvec
    int bitInd = 0;
    // All but the last byte
    for(unsigned int i = 0; i + 1 < dstSizeBytes; i++) {
        unsigned char byte = static_cast<unsigned char>(src[i]);
        for(int b = 0; b < 8; b++) {
                dst[bitInd++] = (byte >> b) & 0x1;
        }
    }
    // Last byte
    unsigned char byte = static_cast<unsigned char>(src[dstSizeBytes - 1]);
    for(int b = 0; (b < 8) && (bitInd < dst.size()); b++) {
            dst[bitInd++] = (byte >> b) & 0x1;
    }
}

void LDPC::vectorToString(const itpp::bvec & src, std::string & dst)
{
    uint32_t srcSizeBits = src.size();
    uint32_t srcSizeBytes = (srcSizeBits + 7) / 8;

    // Resize string to the appropriate size
    dst.resize(srcSizeBytes);

    // Put the bits into the string
    uint32_t bitInd = 0;
    // All but the last byte
    for(unsigned int i = 0; i + 1 < srcSizeBytes; i++) {
        uint8_t byte = 0;
        for(int b = 0; b < 8; b++) {
                byte |= (uint8_t(bool(src[bitInd++])) & 0x1) << b;
        }
        dst[i] = byte;
    }
    // Last byte
    uint8_t byte = 0;
    for(int b = 0; (b < 8) && (bitInd < srcSizeBits); b++) {
            byte |= (uint8_t(bool(src[bitInd++])) & 0x1) << b;
    }
    dst[srcSizeBytes - 1] = byte;
}
