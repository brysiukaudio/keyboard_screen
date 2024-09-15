#include "image_handler.h"
#include "climb.h"
#include "climb_2.h"
//Hack for weird unexplained offset
#define IMG_OFFSET 0


Image_Handler::Image_Handler(Lcd_Comm* lcd) :
    m_lcd(lcd),
    m_currentImageIdx(0),
    m_num_images(2),
    m_image_setup(false),
    m_rab_data(0),
    m_rab_rd_idx(0)
{
    m_images = new Image*[m_num_images];
    m_images[0] = new Image(climb,climb_width,climb_height,climb_size);
    m_images[1] = new Image(climb_2,climb_2_width,climb_2_height,climb_2_size);


    rearrange_buffer = new uint8_t[m_images[m_currentImageIdx]->getImageWidthBytes()];
    // if (m_images[m_currentImageIdx]->getImageWidth() == m_lcd->GetHeight()) {
    //     int32_t orientation = m_lcd->GetOrientation() == PORTRAIT ? LANDSCAPE : PORTRAIT;
    //     m_lcd->SetOrientation(orientation);
    // }
}

Image_Handler::~Image_Handler()
{
    delete [] rearrange_buffer;
}

void Image_Handler::SetupImage() 
{
    uint32_t image_width = m_images[m_currentImageIdx]->getImageWidth();
    uint32_t image_height = m_images[m_currentImageIdx]->getImageHeight();
    uint32_t image_size = m_images[m_currentImageIdx]->getImageSize();

    if (image_width == m_lcd->GetHeight()) {
        int32_t orientation = m_lcd->GetOrientation() == PORTRAIT ? LANDSCAPE : PORTRAIT;
        m_lcd->SetOrientation(orientation);
    }

    if (sizeof(rearrange_buffer) != m_images[m_currentImageIdx]->getImageWidthBytes()) {
        delete [] rearrange_buffer;
        rearrange_buffer = new uint8_t[m_images[m_currentImageIdx]->getImageWidthBytes()];
    }

    uint32_t x0, x1, y0, y1;
    
    m_data_read = 0;
    m_data_left = image_size;

    m_rab_data = 0;
    m_rab_rd_idx = 0;

    x0 = 0;
    y0 = 0;
    x1 = x0 + image_width - 1;
    y1 = y0 + image_height - 1;
    m_lcd->SendCommand(DISPLAY_BITMAP,x0,y0,x1,y1);
}

void Image_Handler::NextImage()
{
    m_currentImageIdx += 1;
    m_currentImageIdx %= m_num_images;
    m_image_setup = false;
}    

void Image_Handler::DisplayImage() 
{
    uint32_t write_available = m_lcd->WriteAvailable();
    uint8_t write_buffer[256];

    uint32_t image_width_bytes = m_images[m_currentImageIdx]->getImageWidthBytes();
    uint8_t* image = m_images[m_currentImageIdx]->getImage();

    if(!m_image_setup)
    {
        SetupImage();
        m_image_setup = true;
        return;
    }

    if (m_rab_data <= 0 && m_data_left > 0) {
        uint8_t temp_buf[image_width_bytes];
        memset(temp_buf,0,image_width_bytes);
        if(m_data_left < image_width_bytes)
        {
            memcpy(temp_buf,&image[m_data_read],m_data_left);
            m_data_read += m_data_left;
            memcpy(rearrange_buffer,&temp_buf[m_data_left-IMG_OFFSET],IMG_OFFSET);
            m_rab_data += IMG_OFFSET;
            memcpy(&rearrange_buffer[m_rab_data],temp_buf,m_data_left-IMG_OFFSET);
            m_rab_data += (m_data_left-IMG_OFFSET);
            m_data_left -= m_data_left;
        } else {
            memcpy(temp_buf,&image[m_data_read],image_width_bytes);
            m_data_read += image_width_bytes;
            memcpy(rearrange_buffer,&temp_buf[image_width_bytes-IMG_OFFSET],IMG_OFFSET);
            m_rab_data += IMG_OFFSET;
            memcpy(&rearrange_buffer[m_rab_data],temp_buf,image_width_bytes-IMG_OFFSET);
            m_rab_data += (image_width_bytes-IMG_OFFSET);
            m_data_left -= image_width_bytes;
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