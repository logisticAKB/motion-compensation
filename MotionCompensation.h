#ifndef MOTION_COMPENSATION_MOTIONCOMPENSATION_H
#define MOTION_COMPENSATION_MOTIONCOMPENSATION_H

#include <string>
#include <fstream>

class MotionCompensation {

public:
    MotionCompensation(const std::string& pathIn, const std::string& pathOut, int width, int height);
    ~MotionCompensation();

    void run(int numThreads=1, const std::string& searchType="full", bool printPSNR=false);

private:
    double calculatePSNR(const unsigned char *frame1, const unsigned char *frame2) const;
    int calculateSAD(const unsigned char *curBlock,
                     int prevFrameY, int prevFrameX,
                     const unsigned char *prevFrame) const;

    void setNewBlock(int y, int x,
                     int newY, int newX,
                     const unsigned char *curBlock,
                     const unsigned char *prevFrame,
                     unsigned char *newFrame) const;

    void fullSearch(int y, int x,
                    const unsigned char *curFrame,
                    const unsigned char *prevFrame,
                    unsigned char *newFrame) const;
    void threeStepSearch(int y, int x,
                         const unsigned char *curFrame,
                         const unsigned char *prevFrame,
                         unsigned char *newFrame) const;

    int _width;
    int _height;
    int _frameSize;
    std::ifstream _inputStream;
    std::ofstream _outputStream;

    const int _blockWidth = 8;
    const int _blockSize = _blockWidth * _blockWidth;

    const int _searchRadiusInBlocks = 2;

    int _blocksPerWidth;
    int _blocksPerHeight;

};


#endif //MOTION_COMPENSATION_MOTIONCOMPENSATION_H
