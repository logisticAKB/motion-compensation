#ifndef MOTION_COMPENSATION_FRAME_H
#define MOTION_COMPENSATION_FRAME_H


class Frame {

public:
    Frame(int width, int height, int dataSize, unsigned char *data);
    ~Frame();

    Frame getBlock(int y, int x, int blockWidth) const;
    void setBlock(int y, int x, const Frame& block);
    unsigned char * getDataPtr() const;
    int getSize() const;
    int getWidth() const;
    int getHeight() const;

    Frame& operator=(const Frame& other);
    bool operator==(const Frame& other) const;

    friend Frame operator-(const Frame& lhs, const Frame& rhs);

private:
    int _width;
    int _height;
    int _dataSize;

    unsigned char  *_data;

};


#endif //MOTION_COMPENSATION_FRAME_H
