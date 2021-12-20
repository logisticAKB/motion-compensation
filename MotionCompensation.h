#ifndef MOTION_COMPENSATION_MOTIONCOMPENSATION_H
#define MOTION_COMPENSATION_MOTIONCOMPENSATION_H

#include <string>
#include <fstream>

#include "Frame.h"

struct MotionVector {
    int deltaY;
    int deltaX;
};

class MotionCompensation {

public:
    MotionCompensation(const std::string& pathIn, const std::string& pathOut, int width, int height);
    ~MotionCompensation();

    void run(int numThreads=1);

    static double calculatePSNR(const Frame& frame1, const Frame& frame2);
    static int calculateSAD(const Frame& frame1, const Frame& frame2);

private:
    void fullSearch(int y, int x, const Frame& curFrame, const Frame& prevFrame, Frame& newFrame) const;

    int _width;
    int _height;
    int _bufferSize;
    unsigned char *_buffer;
    std::ifstream _inputStream;
    std::ofstream _outputStream;

    const int _blockWidth = 8;
    const int _blockSize = _blockWidth * _blockWidth;

    const int _searchRadiusInBlocks = 2;

    int _blocksPerWidth;
    int _blocksPerHeight;

};


#endif //MOTION_COMPENSATION_MOTIONCOMPENSATION_H
