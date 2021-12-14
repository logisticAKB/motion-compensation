#ifndef MOTION_COMPENSATION_MOTIONCOMPENSATION_H
#define MOTION_COMPENSATION_MOTIONCOMPENSATION_H

#include <string>
#include <fstream>

#include "Frame.h"

class MotionCompensation {

public:
    MotionCompensation(const std::string& path, int width, int height);
    ~MotionCompensation();

    void run();

    static double calculatePSNR(const Frame& frame1, const Frame& frame2);

private:
    int _width;
    int _height;
    int _bufferSize;
    unsigned char *_buffer;
    std::ifstream _inputStream;

    const int _blockWidth = 8;
    const int _blockSize = _blockWidth * _blockWidth;

    const int _searchRadius = 4;

    int _blocksPerWidth;
    int _blocksPerHeight;

};


#endif //MOTION_COMPENSATION_MOTIONCOMPENSATION_H
