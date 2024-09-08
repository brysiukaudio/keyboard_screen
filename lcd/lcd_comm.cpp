#include "lcd_comm.h"
#include "tusb.h"

Lcd_Comm::Lcd_Comm(int idx)
{
    com_idx = idx;
}

Lcd_Comm::~Lcd_Comm()
{

}

void Lcd_Comm::SendCommand(uint32_t cmd, uint32_t x, uint32_t y, uint32_t ex, uint32_t ey)
{
    uint8_t byteBuffer[6 + 1]; // +1 for extra null character
    uint32_t const bufsize = sizeof(byteBuffer) - 1;
    if (tuh_cdc_mounted(com_idx)) {
        byteBuffer[0] = (x >> 2);
        byteBuffer[1] = (((x & 3) << 6) + (y >> 4));
        byteBuffer[2] = (((y & 15) << 4) + (ex >> 6));
        byteBuffer[3] = (((ex & 63) << 2) + (ey >> 8));
        byteBuffer[4] = (ey & 255);
        byteBuffer[5] = cmd;
        tuh_cdc_write(com_idx, byteBuffer, bufsize);
        tuh_cdc_write_flush(com_idx);
    }
}
