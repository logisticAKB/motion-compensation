#include <iostream>
#include <cmath>

#include "MotionCompensation.h"
#include "Frame.h"

#include <opencv2/opencv.hpp>
using namespace cv;

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

void MotionCompensation::run() {
    _inputStream.read((char *)_buffer, _bufferSize);
    Frame prevFrame(_width, (int)(_height * 1.5), _bufferSize, _buffer);

    while(_inputStream.read((char *)_buffer, _bufferSize)) {
        Frame curFrame(_width, (int)(_height * 1.5), _bufferSize, _buffer);
        if (prevFrame == curFrame) continue;



        Mat img(_height + _height/2, _width, CV_8U, _buffer);

        Mat img_rgb(_height, _width, CV_8UC3);
        cvtColor(img, img_rgb, COLOR_YUV2RGBA_YV12, 3);

        imshow ("RAW", img_rgb);
        if(waitKey(30) >= 0) break;



        std::cout << "----- NEW FRAME -----" << (prevFrame == curFrame) << std::endl;
        for (int y = 0; y < _blocksPerHeight; y++) {
            for (int x = 0; x < _blocksPerWidth; x++) {
                Frame curBlock = curFrame.getBlock(y, x, _blockWidth);

                int searchFromY = std::max(y - _searchRadius, 0);
                int searchToY = std::min(y + _searchRadius, _blocksPerHeight - 1);

                int searchFromX = std::max(x - _searchRadius, 0);
                int searchToX = std::min(x + _searchRadius, _blocksPerWidth - 1);

                double bestScore = -1;
                int bestPrevY = -1;
                int bestPrevX = -1;
                for (int prevFrameY = searchFromY; prevFrameY <= searchToY; prevFrameY++) {
                    for (int prevFrameX = searchFromX; prevFrameX <= searchToX; prevFrameX++) {
                        Frame prevBlock = prevFrame.getBlock(prevFrameY, prevFrameX, _blockWidth);

                        double score = calculatePSNR(curBlock, prevBlock);
                        if (score > bestScore) {
                            bestScore = score;
                            bestPrevY = prevFrameY;
                            bestPrevX = prevFrameX;
                        }
                    }
                }
            }
        }

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
