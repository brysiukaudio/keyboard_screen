#include "image_handler.h"
//Hack for weird unexplained offset
#define IMG_OFFSET 58
#define BYTES_PER_PIXEL 2

Image_Handler::Image_Handler(Lcd_Comm* lcd, uint8_t* image, uint32_t image_width, uint32_t image_height, uint32_t image_size) :
    m_lcd(lcd),
    m_image(image),
    m_image_width(image_width),
    m_image_width_bytes(image_width*BYTES_PER_PIXEL),
    m_image_height(image_height),
    m_image_size(image_size),
    m_image_setup(false),
    m_rab_data(0),
    m_rab_rd_idx(0)
{
    rearrange_buffer = new uint8_t[m_image_width_bytes];
    if (m_image_width == m_lcd->GetHeight()) {
        int32_t orientation = m_lcd->GetOrientation() == PORTRAIT ? LANDSCAPE : PORTRAIT;
        m_lcd->SetOrientation(orientation);
    }
}

Image_Handler::~Image_Handler()
{
    delete [] rearrange_buffer;
}

void Image_Handler::SetupImage() 
{
    uint32_t x0, x1, y0, y1;
    
    m_data_read = 0;
    m_data_left = m_image_size;

    x0 = 0;
    y0 = 0;
    x1 = x0 + m_image_width - 1;
    y1 = y0 + m_image_height - 1;
    m_lcd->SendCommand(DISPLAY_BITMAP,x0,y0,x1,y1);
}

void Image_Handler::DisplayImage() 
{
    uint32_t write_available = m_lcd->WriteAvailable();
    uint8_t write_buffer[256];
    if(!m_image_setup)
    {
        SetupImage();
        m_image_setup = true;
        return;
    }

    if (m_rab_data <= 0 && m_data_left > 0) {
        uint8_t temp_buf[m_image_width_bytes];
        memset(temp_buf,0,m_image_width_bytes);
        if(m_data_left < m_image_width_bytes)
        {
            memcpy(temp_buf,&m_image[m_data_read],m_data_left);
            m_data_read += m_data_left;
            memcpy(rearrange_buffer,&temp_buf[m_data_left-IMG_OFFSET],IMG_OFFSET);
            m_rab_data += IMG_OFFSET;
            memcpy(&rearrange_buffer[m_rab_data],temp_buf,m_data_left-IMG_OFFSET);
            m_rab_data += (m_data_left-IMG_OFFSET);
            m_data_left -= m_data_left;
        } else {
            memcpy(temp_buf,&m_image[m_data_read],m_image_width_bytes);
            m_data_read += m_image_width_bytes;
            memcpy(rearrange_buffer,&temp_buf[m_image_width_bytes-IMG_OFFSET],IMG_OFFSET);
            m_rab_data += IMG_OFFSET;
            memcpy(&rearrange_buffer[m_rab_data],temp_buf,m_image_width_bytes-IMG_OFFSET);
            m_rab_data += (m_image_width_bytes-IMG_OFFSET);
            m_data_left -= m_image_width_bytes;
        }
        m_rab_rd_idx = 0;
    }

    if (m_rab_data > 0 && write_available > 0) {
        if (m_rab_data < write_available ) {
            memset(write_buffer,0,256);
            memcpy(write_buffer,&rearrange_buffer[m_rab_rd_idx],m_rab_data);
            m_rab_rd_idx += m_rab_data;
            m_lcd->WriteData(write_buffer,m_rab_data, false);
            m_rab_data -= m_rab_data;
        } else{
            memset(write_buffer,0,256);
            memcpy(write_buffer,&rearrange_buffer[m_rab_rd_idx],write_available);
            m_rab_rd_idx += (write_available);
            m_rab_data -= (write_available);
            m_lcd->WriteData(write_buffer,write_available, false);
        }
    }
}