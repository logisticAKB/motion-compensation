#include <fstream>
#include <vector>
#include <iostream>
#include <chrono>

#include <mutex>
#include <condition_variable>

#include "MotionCompensation.h"
#include "Frame.h"
#include "ThreadPool.h"

class Test {
public:
    void test(int x, int y) {
        size_t id = std::hash<std::thread::id>{}(std::this_thread::get_id());

//        std::cout << "Thread " << id << " sleeping for " << x << "sec" << std::endl;

        std::this_thread::sleep_for(std::chrono::microseconds (x));

//        std::cout << "Thread " << id << " finished" << std::endl;
    }
};

int main() {
//    srand ( time(NULL) );
//
//    std::mutex mx;
//    std::condition_variable cond;
//    ThreadPool pool(12, &cond);
//
//    Test t;
//
//    for (int i = 0; i < 1000; i++) {
//        pool.add(std::bind(&Test::test, t, 100, 1));
//
//    }
//
//    {
//        std::unique_lock<std::mutex> lock(mx);
//        cond.wait(lock, [&pool]{return !pool.isProcessing();});
//    }
//
//    std::cout << "here" << std::endl;



//    auto *data = new unsigned char[16]{1,2,3,4,
//                                       5,6,7,8,
//                                       9,10,11,12,
//                                       13,14,15,16};
//
//    Frame frame(4, 4, 16, data);
//
//    auto *data1 = new unsigned char[16]{1,2,3,4,
//                                       5,6,7,8,
//                                       9,112,11,12,
//                                       13,14,15,17};
//
//    Frame frame1(4, 4, 16, data1);
//
//    std::cout << MotionCompensation::calculateSAD(frame, frame1) << std::endl;

//    auto subFrame = frame.getBlock(1, 1, 2);
//
//    for (int i = 0; i < 4; i++) {
//        std::cout << (int)subFrame.getDataPtr()[i] << ' ';
//    }

    //
//    auto *data2 = new unsigned char[4]{101, 102,
//                                       103, 104};
//
//    Frame frame2(2, 2, 4, data2);
//
//    frame.setBlock(0, 0, frame2);
//
//    auto frame3 = frame - frame2;
//
//    std::cout << (frame == frame2) << std::endl;

//
//    std::cout << MotionCompensation::calculatePSNR(frame, frame) << std::endl;

//
//
//
//    for (int i = 0; i < 4; i++) {
//        for (int j = 0; j < 4; j++) {
//            std::cout << (int)frame.getDataPtr()[i * 4 + j] << ' ';
//        }
//        std::cout << std::endl;
//    }



    MotionCompensation mc("../video.yuv420p", "../output.yuv420p", 1920, 1080);
    mc.run(12);

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