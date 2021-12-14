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

Frame::~Frame() {
    delete[] _data;
}