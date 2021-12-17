#include <cstring>
#include <iostream>
#include "Frame.h"

Frame::Frame(int width, int height, int dataSize, unsigned char *data) {
    _width = width;
    _height = height;
    _dataSize = dataSize;

    _data = new unsigned char [_dataSize];
    memcpy(_data, data, _dataSize);
}

Frame::Frame(int width, int height, int dataSize) {
    _width = width;
    _height = height;
    _dataSize = dataSize;

    _data = new unsigned char [_dataSize];
    memset(_data, 0, _dataSize);
}

Frame::~Frame() {
    delete[] _data;
    _data = nullptr;
}

Frame Frame::getBlock(int y, int x, int blockWidth) const {
    int yOffset = y * blockWidth;
    int xOffset = x * blockWidth;

    int blockSize = blockWidth * blockWidth;

    auto *blockBuffer = new unsigned char[blockSize];
    for (int i = 0; i < blockWidth; i++) {
        memcpy(blockBuffer + i * blockWidth, _data + ((_width * (yOffset + i)) + xOffset), blockWidth);
    }

    Frame block(blockWidth, blockWidth, blockSize, blockBuffer);

    delete[] blockBuffer;
    blockBuffer = nullptr;

    return block;
}

void Frame::setBlock(int y, int x, const Frame &block) {
    int yOffset = y * block.getHeight();
    int xOffset = x * block.getWidth();

    for (int i = 0; i < block.getHeight(); i++) {
        memcpy(_data + ((_width * (yOffset + i)) + xOffset), block.getDataPtr() + i * block.getWidth(), block.getWidth());
    }
}

unsigned char *Frame::getDataPtr() const {
    return _data;
}

int Frame::getSize() const {
    return _dataSize;
}

int Frame::getWidth() const {
    return _width;
}

int Frame::getHeight() const {
    return _height;
}

Frame &Frame::operator=(const Frame &other) {
    if (this == &other)
        return *this;

    _width = other._width;
    _height = other._height;
    _dataSize = other._dataSize;

    delete[] _data;
    _data = nullptr;
    _data = new unsigned char [_dataSize];
    memcpy(_data, other._data, _dataSize);

    return *this;
}

bool Frame::operator==(const Frame &other) const {
    for (int i = 0; i < _dataSize; i++) {
        if (_data[i] != other._data[i]) {
            return false;
        }
    }

    return true;
}

Frame operator-(const Frame &lhs, const Frame &rhs) {
    auto *buffer = new unsigned char[lhs.getSize()];
    memcpy(buffer, lhs.getDataPtr(), lhs.getSize());

    for (int i = 0; i < lhs.getSize(); i++) {
        buffer[i] = abs((int)lhs.getDataPtr()[i] - (int)rhs.getDataPtr()[i]);
    }

    Frame result(lhs.getWidth(), lhs.getHeight(), lhs.getSize(), buffer);

    delete[] buffer;
    buffer = nullptr;

    return result;
}
