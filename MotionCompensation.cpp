#include <iostream>
#include <cmath>
#include <climits>
#include <functional>

#include "MotionCompensation.h"
#include "ThreadPool.h"

const int blockWidth = 8;
const int blockSize = 64;
const int searchRadiusInBlocks = 2;

MotionCompensation::MotionCompensation(const std::string& pathIn, const std::string& pathOut, int width, int height) {
    _width = width;
    _height = (int)(height * 1.5);
    _frameSize = _width * _height;
    _inputStream.open(pathIn, std::ios::binary);
    _outputStream.open(pathOut, std::ios::binary);

    _blocksPerWidth = _width / blockWidth;
    _blocksPerHeight = _height / blockWidth;
}

MotionCompensation::~MotionCompensation() {
    _inputStream.close();
    _outputStream.close();
}

void MotionCompensation::run(int numThreads, const std::string& searchType, bool printPSNR) {
    auto *prevFrame = new unsigned char [_frameSize];
    auto *curFrame = new unsigned char [_frameSize];
    auto *newFrame = new short [_frameSize];

    ThreadPool pool(numThreads);

    _inputStream.read((char *)prevFrame, _frameSize);

    while(_inputStream.read((char *)curFrame, _frameSize)) {
        if (printPSNR)
            std::cout << "PSNR for previous and current frame: " << calculatePSNR(prevFrame, curFrame) << std::endl;

        for (int y = 0; y < _blocksPerHeight; y++) {
            for (int x = 0; x < _blocksPerWidth; x++) {
                if (searchType == "full") {
                    pool.add(std::bind(&MotionCompensation::fullSearch, this, y, x, curFrame, prevFrame, newFrame));
                } else {
                    pool.add(std::bind(&MotionCompensation::threeStepSearch, this, y, x, curFrame, prevFrame, newFrame));
                }
            }
        }

        while (pool.isProcessing()) {}

        _outputStream.write(reinterpret_cast<const char*>(newFrame), std::streamsize(_frameSize * sizeof(short)));

        std::copy(curFrame, curFrame + _frameSize, prevFrame);
    }

    delete[] prevFrame;
    delete[] curFrame;
    delete[] newFrame;
}

double MotionCompensation::calculatePSNR(const unsigned char *frame1, const unsigned char *frame2) const {
//    long sum = 0;
//    for (int i = 0; i < _frameSize; i++) {
//        int diff = (int)frame1[i] - (int)frame2[i];
//        sum += (diff * diff);
//    }

    long sum = 0;
    for (int i = 0; i < _frameSize; i += 8)  {
        for (int j = 0; j < 8; ++j) {
            int diff = (int)frame1[i + j] - (int)frame2[i + j];
            sum += (diff * diff);
        }
    }

    for (int i = _frameSize - (_frameSize % 8); i < _frameSize; ++i) {
        int diff = (int)frame1[i] - (int)frame2[i];
        sum += (diff * diff);
    }

    double mse = (double)sum / _frameSize;
    double psnr = 10 * log10(255 * 255 / mse);

    return psnr;
}

void MotionCompensation::fullSearch(int y, int x,
                                    const unsigned char *curFrame,
                                    const unsigned char *prevFrame,
                                    short *newFrame) const {
    unsigned char curBlock[blockSize];

    int blockYOffset = y * blockWidth;
    int blockXOffset = x * blockWidth;

    for (int i = 0; i < blockWidth; ++i) {
        for (int j = 0; j < blockWidth; ++j) {
            curBlock[i * blockWidth + j] = curFrame[_width * (blockYOffset + i) + (blockXOffset + j)];
        }
    }

    int searchFromY = std::max(y - searchRadiusInBlocks, 0);
    int searchToY = std::min(y + searchRadiusInBlocks, _blocksPerHeight - 1);

    int searchFromX = std::max(x - searchRadiusInBlocks, 0);
    int searchToX = std::min(x + searchRadiusInBlocks, _blocksPerWidth - 1);

    int bestSad = INT_MAX;
    int bestPrevY = -1;
    int bestPrevX = -1;
    for (int prevFrameY = searchFromY; prevFrameY <= searchToY; prevFrameY++) {
        for (int prevFrameX = searchFromX; prevFrameX <= searchToX; prevFrameX++) {
            int sad = calculateSAD(curBlock, prevFrameY, prevFrameX, prevFrame);
            if (sad < bestSad) {
                bestSad = sad;
                bestPrevY = prevFrameY;
                bestPrevX = prevFrameX;
            }
        }
    }

    setNewBlock(y, x, bestPrevY, bestPrevX, curBlock, prevFrame, newFrame);
}

void MotionCompensation::threeStepSearch(int y, int x,
                                         const unsigned char *curFrame,
                                         const unsigned char *prevFrame,
                                         short *newFrame) const {
    int stepSize = 4;
    std::vector<std::pair<int, int>> d = { {-1, -1}, {-1, 0}, {-1, 1},
                                           {0, -1}, {0, 0}, {0, 1},
                                           {1, -1}, {1, 0}, {1, 1} };
    unsigned char curBlock[blockSize];

    int blockYOffset = y * blockWidth;
    int blockXOffset = x * blockWidth;

    for (int i = 0; i < blockWidth; ++i) {
        for (int j = 0; j < blockWidth; ++j) {
            curBlock[i * blockWidth + j] = curFrame[_width * (blockYOffset + i) + (blockXOffset + j)];
        }
    }

    int newY = y;
    int newX = x;
    while (stepSize != 1) {
        int bestSad = INT_MAX;
        int bestY = -1;
        int bestX = -1;

        for (auto k : d) {
            int prevFrameY = k.first * stepSize + newY;
            int prevFrameX = k.second * stepSize + newX;

            if (prevFrameY >= 0 && prevFrameY < _blocksPerHeight &&
                    prevFrameX >= 0 && prevFrameX < _blocksPerWidth) {

                int sad = calculateSAD(curBlock, prevFrameY, prevFrameX, prevFrame);
                if (sad < bestSad) {
                    bestSad = sad;
                    bestY = prevFrameY;
                    bestX = prevFrameX;
                }
            }
        }

        newY = bestY;
        newX = bestX;
        stepSize /= 2;
    }

    setNewBlock(y, x, newY, newX, curBlock, prevFrame, newFrame);
}

void MotionCompensation::setNewBlock(int y, int x,
                                     int newY, int newX,
                                     const unsigned char *curBlock,
                                     const unsigned char *prevFrame,
                                     short *newFrame) const {
    int curBlockYOffset = y * blockWidth;
    int curBlockXOffset = x * blockWidth;

    int prevBlockYOffset = newY * blockWidth;
    int prevBlockXOffset = newX * blockWidth;

    for (int i = 0; i < blockWidth; ++i) {
        for (int j = 0; j < blockWidth; ++j) {
            newFrame[_width * (curBlockYOffset + i) + (curBlockXOffset + j)] =
                    (short)((short)curBlock[i * blockWidth + j] - (short)prevFrame[_width * (prevBlockYOffset + i) + (prevBlockXOffset + j)]);
        }
    }
}

int MotionCompensation::calculateSAD(const unsigned char *curBlock,
                                     int prevFrameY, int prevFrameX,
                                     const unsigned char *prevFrame) const {
    int prevBlock2YOffset = prevFrameY * blockWidth;
    int prevBlock2XOffset = prevFrameX * blockWidth;

    int sum = 0;
    for (int i = 0; i < blockWidth; ++i) {
        for (int j = 0; j < blockWidth; ++j) {
            sum += abs((int)curBlock[i * blockWidth + j] -
                       (int)prevFrame[_width * (prevBlock2YOffset + i) + (prevBlock2XOffset + j)]);
        }
    }

    return sum;
}