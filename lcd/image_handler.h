#pragma once

#include "pico/stdlib.h"
#include "string.h"
#include "lcd_comm.h"
#include "image.h"

class Image_Handler{
public:
    Image_Handler(Lcd_Comm* lcd);
    ~Image_Handler();
    void DisplayImage();
    void NextImage();
private:

    void SetupImage();
    Lcd_Comm* m_lcd;

    Image ** m_images;
    uint32_t m_num_images;
    uint32_t m_currentImageIdx;

    uint8_t* rearrange_buffer;
    uint32_t m_rab_data;
    uint32_t m_rab_rd_idx;

    uint32_t m_data_left;
    uint32_t m_data_read;
    bool m_image_setup;
};