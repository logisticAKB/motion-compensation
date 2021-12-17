#include <iostream>
#include <cmath>
#include <atomic>
#include <thread>
#include <condition_variable>

#include "MotionCompensation.h"
#include "Frame.h"

#include <opencv2/opencv.hpp>

using namespace cv;

std::atomic<int> nThreads;
std::mutex cv_m;
std::condition_variable cond;

MotionCompensation::MotionCompensation(const std::string& path, int width, int height) {
    _width = width;
    _height = height;
    _bufferSize = (int)(_width * _height * 1.5);
    _buffer = new unsigned char [_bufferSize];
    _inputStream.open(path, std::ios::binary);

    _blocksPerWidth = _width / _blockWidth;
    _blocksPerHeight = _height / _blockWidth;
}

MotionCompensation::~MotionCompensation() {
    _inputStream.close();
    delete[] _buffer;
    _buffer = nullptr;
}

void MotionCompensation::run(int threadsNum) {
    nThreads = threadsNum;

    _inputStream.read((char *)_buffer, _bufferSize);
    Frame prevFrame(_width, (int)(_height * 1.5), _bufferSize, _buffer);

    while(_inputStream.read((char *)_buffer, _bufferSize)) {
        Frame curFrame(_width, (int)(_height * 1.5), _bufferSize, _buffer);

        Frame newFrame(_width, (int)(_height * 1.5), _bufferSize);

        if (prevFrame == curFrame) continue;

        for (int y = 0; y < _blocksPerHeight; y++) {
            for (int x = 0; x < _blocksPerWidth; x++) {
                std::unique_lock<std::mutex> lk(cv_m);
                cond.wait(lk, []{return nThreads > 0;});

//                fullSearch(y, x, curFrame, prevFrame, newFrame);

                std::thread worker(&MotionCompensation::fullSearch, this, y, x, std::ref(curFrame), std::ref(prevFrame), std::ref(newFrame));
                worker.join();
//                std::thread worker{&MotionCompensation::fullSearch, this, y, x, std::ref(curFrame), std::ref(prevFrame), std::ref(newFrame)};
            }
        }

        Mat img2(_height + _height/2, _width, CV_8U, newFrame.getDataPtr());
        Mat img_rgb2(_height, _width, CV_8UC3);
        cvtColor(img2, img_rgb2, COLOR_YUV2RGBA_YV12, 3);
        imshow("comp", img_rgb2);
        if(waitKey(30) >= 0) break;

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
    std::cout << std::this_thread::get_id() << std::endl;

    nThreads--;

    Frame curBlock = curFrame.getBlock(y, x, _blockWidth);

    int searchFromY = std::max(y - _searchRadiusInBlocks, 0);
    int searchToY = std::min(y + _searchRadiusInBlocks, _blocksPerHeight - 1);

    int searchFromX = std::max(x - _searchRadiusInBlocks, 0);
    int searchToX = std::min(x + _searchRadiusInBlocks, _blocksPerWidth - 1);

    double bestScore = -1;
    Frame bestPrevBlock(_blockWidth, _blockWidth, _blockSize);
    for (int prevFrameY = searchFromY; prevFrameY <= searchToY; prevFrameY++) {
        for (int prevFrameX = searchFromX; prevFrameX <= searchToX; prevFrameX++) {
            Frame prevBlock = prevFrame.getBlock(prevFrameY, prevFrameX, _blockWidth);

            double score = calculatePSNR(curBlock, prevBlock);
            if (score > bestScore) {
                bestScore = score;
                bestPrevBlock = prevBlock;
            }
        }
    }

    newFrame.setBlock(y, x, curBlock - bestPrevBlock);

    nThreads++;
    cond.notify_all();
}

//Mat img2(_height + _height/2, _width, CV_8U, newFrame.getDataPtr());
//Mat img_rgb2(_height, _width, CV_8UC3);
//cvtColor(img2, img_rgb2, COLOR_YUV2RGBA_YV12, 3);
//imshow("comp", img_rgb2);
//if(waitKey(30) >= 0) break;