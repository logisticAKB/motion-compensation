#ifndef MOTION_COMPENSATION_FRAME_H
#define MOTION_COMPENSATION_FRAME_H


class Frame {

public:
    Frame(int width, int height, int dataSize, unsigned char *data);
    ~Frame();

private:
    int _width;
    int _height;
    int _dataSize;

    unsigned char  *_data;

};


#endif //MOTION_COMPENSATION_FRAME_H
