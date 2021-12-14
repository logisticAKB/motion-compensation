#include <iostream>

#include "MotionCompensation.h"
#include "Frame.h"

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
}

void MotionCompensation::run() {
    while(_inputStream.read((char *)_buffer, _bufferSize)) {
        Frame frame(_width, _height, _bufferSize, _buffer);

        for (int y = 0; y < _blocksPerHeight; y++) {
            for (int x = 0; x < _blocksPerWidth; x++) {
                
            }
        }
    }
}
