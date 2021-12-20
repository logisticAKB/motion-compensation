#include <iostream>
#include <cmath>
#include <climits>

#include "MotionCompensation.h"
#include "Frame.h"
#include "ThreadPool.h"

MotionCompensation::MotionCompensation(const std::string& pathIn, const std::string& pathOut, int width, int height) {
    _width = width;
    _height = height;
    _bufferSize = (int)(_width * _height * 1.5);
    _buffer = new unsigned char [_bufferSize];
    _inputStream.open(pathIn, std::ios::binary);
    _outputStream.open(pathOut, std::ios::binary);

    _blocksPerWidth = _width / _blockWidth;
    _blocksPerHeight = (int)(_height * 1.5) / _blockWidth;
}

MotionCompensation::~MotionCompensation() {
    _inputStream.close();
    _outputStream.close();
    delete[] _buffer;
    _buffer = nullptr;
}

void MotionCompensation::run(int numThreads, const std::string& searchType, bool printPSNR) {
    std::mutex mx;
    std::condition_variable nextFrameCond;
    ThreadPool pool(numThreads, &nextFrameCond);

    _inputStream.read((char *)_buffer, _bufferSize);
    Frame prevFrame(_width, (int)(_height * 1.5), _bufferSize, _buffer);

    while(_inputStream.read((char *)_buffer, _bufferSize)) {
        Frame curFrame(_width, (int)(_height * 1.5), _bufferSize, _buffer);

        Frame newFrame(_width, (int)(_height * 1.5), _bufferSize);

        // Identical frame skipping. Not a part of algorithm. Just for this example
        if (prevFrame == curFrame) continue;

        if (printPSNR)
            std::cout << "PSNR for previous and current frame: " << calculatePSNR(prevFrame, curFrame) << std::endl;

        for (int y = 0; y < _blocksPerHeight; y++) {
            for (int x = 0; x < _blocksPerWidth; x++) {
                if (searchType == "full") {
                    pool.add(std::bind(&MotionCompensation::fullSearch, this,
                                       y, x, std::cref(curFrame), std::cref(prevFrame), std::ref(newFrame)));
                } else {
                    pool.add(std::bind(&MotionCompensation::threeStepSearch, this,
                                       y, x, std::cref(curFrame), std::cref(prevFrame), std::ref(newFrame)));
                }
            }
        }

        {
            std::unique_lock<std::mutex> lock(mx);
            nextFrameCond.wait(lock, [&pool]{return !pool.isProcessing();});
        }

        _outputStream.write((char *)newFrame.getDataPtr(), newFrame.getSize());

        prevFrame = curFrame;
    }
}

double MotionCompensation::calculatePSNR(const Frame& frame1, const Frame& frame2) {
    auto frameData1 = frame1.getDataPtr();
    auto frameData2 = frame2.getDataPtr();

    long sum = 0;
    for (int i = 0; i < frame1.getSize(); i++) {
        int diff = (int)frameData1[i] - (int)frameData2[i];
        sum += (diff * diff);
    }

    double mse = (double)sum / frame1.getSize();
    double psnr = 10 * log10(255 * 255 / mse);

    return psnr;
}

void MotionCompensation::fullSearch(int y, int x, const Frame& curFrame, const Frame& prevFrame, Frame& newFrame) const {
    Frame curBlock = curFrame.getBlock(y, x, _blockWidth);

    int searchFromY = std::max(y - _searchRadiusInBlocks, 0);
    int searchToY = std::min(y + _searchRadiusInBlocks, _blocksPerHeight - 1);

    int searchFromX = std::max(x - _searchRadiusInBlocks, 0);
    int searchToX = std::min(x + _searchRadiusInBlocks, _blocksPerWidth - 1);

    int bestSad = INT_MAX;
    Frame bestPrevBlock(_blockWidth, _blockWidth, _blockSize);
    for (int prevFrameY = searchFromY; prevFrameY <= searchToY; prevFrameY++) {
        for (int prevFrameX = searchFromX; prevFrameX <= searchToX; prevFrameX++) {
            Frame prevBlock = prevFrame.getBlock(prevFrameY, prevFrameX, _blockWidth);

            int sad = calculateSAD(curBlock, prevBlock);
            if (sad < bestSad) {
                bestSad = sad;
                bestPrevBlock = prevBlock;
            }
        }
    }

    newFrame.setBlock(y, x, curBlock - bestPrevBlock);
}

void MotionCompensation::threeStepSearch(int y, int x, const Frame &curFrame, const Frame &prevFrame,
                                         Frame &newFrame) const {
    Frame curBlock = curFrame.getBlock(y, x, _blockWidth);

    int stepSize = 4;
    std::vector<std::pair<int, int>> d = { {-1, -1}, {-1, 0}, {-1, 1},
                                           {0, -1}, {0, 0}, {0, 1},
                                           {1, -1}, {1, 0}, {1, 1} };
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
                Frame prevBlock = prevFrame.getBlock(prevFrameY, prevFrameX, _blockWidth);

                int sad = calculateSAD(curBlock, prevBlock);
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
    newFrame.setBlock(y, x, curBlock - prevFrame.getBlock(newY, newX, _blockWidth));
}

int MotionCompensation::calculateSAD(const Frame &frame1, const Frame &frame2) {
    auto frameData1 = frame1.getDataPtr();
    auto frameData2 = frame2.getDataPtr();

    int sum = 0;
    for (int i = 0; i < frame1.getSize(); i++) {
        sum += abs((int)frameData1[i] - (int)frameData2[i]);
    }

    return sum;
}