#include "lcd_comm.h"
#include "tusb.h"

Lcd_Comm::Lcd_Comm(int idx)
{
    m_com_idx = idx;
    m_display_width = 320;
    m_display_height = 480;
    m_orientation = PORTRAIT;
}

Lcd_Comm::~Lcd_Comm()
{
}

uint32_t Lcd_Comm::GetWidth() 
{
    if (m_orientation == PORTRAIT || m_orientation == REVERSE_PORTRAIT) {
        return m_display_width;
    } else {
        return m_display_height;
    }
}

uint32_t Lcd_Comm::GetHeight()
{
    if (m_orientation == PORTRAIT || m_orientation == REVERSE_PORTRAIT) {
        return m_display_height;
    } else {
        return m_display_width;
    }
}

void Lcd_Comm::SendCommand(uint32_t cmd, uint32_t x, uint32_t y, uint32_t ex, uint32_t ey)
{
    uint8_t byteBuffer[6];
    uint32_t const bufsize = sizeof(byteBuffer);
    if (tuh_cdc_mounted(m_com_idx)) {
        byteBuffer[0] = (x >> 2);
        byteBuffer[1] = (((x & 3) << 6) + (y >> 4));
        byteBuffer[2] = (((y & 15) << 4) + (ex >> 6));
        byteBuffer[3] = (((ex & 63) << 2) + (ey >> 8));
        byteBuffer[4] = (ey & 255);
        byteBuffer[5] = cmd;
        WriteData(byteBuffer,bufsize);
    }
}

uint32_t Lcd_Comm::WriteAvailable() 
{ 
    return tuh_cdc_write_available(m_com_idx); 
}
void Lcd_Comm::WriteData(uint8_t* data, uint32_t data_size, bool flush) {
    if (tuh_cdc_mounted(m_com_idx)) {
        tuh_cdc_write(m_com_idx, data, data_size);
        if (flush) {
            uint32_t bytes_flushed = tuh_cdc_write_flush(m_com_idx);
        }
    }
}

void Lcd_Comm::SetOrientation(uint32_t orientation) 
{
    uint8_t byteBuffer[11];
    uint32_t bufsize = sizeof(byteBuffer);
    uint32_t width, height, x, y, ex, ey;
    m_orientation = orientation;
    width = GetWidth();
    height = GetHeight();
    x = 0;
    y = 0;
    ex = 0;
    ey = 0;
    byteBuffer[0] = (x >> 2);
    byteBuffer[1] = (((x & 3) << 6) + (y >> 4));
    byteBuffer[2] = (((y & 15) << 4) + (ex >> 6));
    byteBuffer[3] = (((ex & 63) << 2) + (ey >> 8));
    byteBuffer[4] = (ey & 255);
    byteBuffer[5] = SET_ORIENTATION;
    byteBuffer[6] = (orientation + 100);
    byteBuffer[7] = (width >> 8);
    byteBuffer[8] = (width & 255);
    byteBuffer[9] = (height >> 8);
    byteBuffer[10] = (height & 255);
    WriteData(byteBuffer,bufsize);
}