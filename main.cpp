#include <fstream>
#include <vector>
#include <iostream>

#include "MotionCompensation.h"

int main() {
    MotionCompensation mc("/home/alexey/Documents/video.yuv420p", 1920, 1080);
    mc.run();

//    std::ifstream input( "/home/alexey/Documents/video.yuv420p", std::ios::binary);
//
////    std::vector<unsigned char> buffer(1920*1080*3 / 2);
//    auto *buffer = new unsigned char [1920*1080*3 / 2];
//    int i = 0;
////    while(input.read((char *)&buffer[0],1920*1080*3/2)){
//    while(input.read((char *)buffer,1920*1080*3/2)){
//
////        Frame frame(1, 1, buffer);
//
//
//        std::cout << (int)buffer[0] << std::endl;
//        i++;
//        break;
//    }
//    std::cout << i << std::endl;

//    std::ofstream output( "/home/alexey/Documents/video2.mp4", std::ios::binary );

//    int len = 1920 * 1080 * 2;
//
//    char* buffer = new char[len];

//    input.read(buffer, len);

//    std::copy(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>(), std::ostreambuf_iterator<char>(output));

//    std::istreambuf_iterator<char> it(input);


//    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

//    std::cout << buffer.size() << std::endl;

    return 0;
}

//#include <iostream>
//#include <fstream>
//#include <cstdint>
//
//bool read32(std::ifstream& f, uint32_t& n) {
//    f.read((char*)&n, sizeof n);
//    // Comment this out if your machine is big-endian.
//    n = n >> 24 | n << 24 | (n >> 8 & 0xff00) | (n << 8 & 0xff0000);
//    return bool(f);
//}
//
//void print_chars(uint32_t n) {    // assumes little-endian
//    std::cout << (char)(n >> 24)
//              << (char)(n >> 16 & 0xff)
//              << (char)(n >>  8 & 0xff)
//              << (char)(n       & 0xff);
//}
//
//int main() {
//    std::ifstream f("/home/alexey/Documents/video.mp4", std::ifstream::in|std::ifstream::binary);
//    if (!f) { std::cerr << "Cannot open file.\n"; return 1; }
//    for (uint32_t size; read32(f, size); ) {
//        uint32_t type;
//        read32(f, type);
//        print_chars(type);
//        std::cout << "  " << size << '\n';
//        f.ignore(size - 8);
//    }
//}