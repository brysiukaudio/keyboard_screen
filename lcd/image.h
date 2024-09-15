#pragma once

#define BYTES_PER_PIXEL 2

class Image{
public:
    Image(uint8_t* image, uint32_t image_width, uint32_t image_height, uint32_t image_size) :
        m_image(image),
        m_image_width(image_width),
        m_image_width_bytes(image_width*BYTES_PER_PIXEL),
        m_image_height(image_height),
        m_image_size(image_size)
    {
    }
    ~Image();

    uint8_t* getImage() { return m_image; };
    uint32_t getImageWidth() { return m_image_width; };
    uint32_t getImageWidthBytes() {return m_image_width_bytes; };
    uint32_t getImageHeight() { return m_image_height; };
    uint32_t getImageSize() {return m_image_size; };

private:
    uint8_t* m_image;
    uint32_t m_image_size;
    uint32_t m_image_width;
    uint32_t m_image_width_bytes;
    uint32_t m_image_height;
};