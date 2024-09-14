#pragma once

#include "pico/stdlib.h"
#include "string.h"
#include "lcd_comm.h"

class Image_Handler{
public:
    Image_Handler(Lcd_Comm* lcd, uint8_t* image, uint32_t image_width, uint32_t image_height, uint32_t image_size);
    ~Image_Handler();
    void DisplayImage();
private:

    void SetupImage();
    Lcd_Comm* m_lcd;
    uint8_t* m_image;
    uint32_t m_image_size;
    uint32_t m_image_width;
    uint32_t m_image_width_bytes;
    uint32_t m_image_height;

    uint8_t* rearrange_buffer;
    uint32_t m_rab_data;
    uint32_t m_rab_rd_idx;

    uint32_t m_data_left;
    uint32_t m_data_read;
    bool m_image_setup;
};