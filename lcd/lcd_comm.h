#pragma once
#include "pico/stdlib.h"

enum Orientation {
    PORTRAIT = 0,
    LANDSCAPE = 2,
    REVERSE_PORTRAIT = 1,
    REVERSE_LANDSCAPE = 3,
};

enum Command {
    RESET = 101, // Resets the display
    CLEAR = 102,  // Clears the display to a white screen
    TO_BLACK = 103,  // Makes the screen go black. NOT TESTED
    SCREEN_OFF = 108,  // Turns the screen off
    SCREEN_ON = 109,  // Turns the screen on
    SET_BRIGHTNESS = 110,  // Sets the screen brightness
    SET_ORIENTATION = 121,  // Sets the screen orientation
    DISPLAY_BITMAP = 197,  // Displays an image on the screen
};

class Lcd_Comm{
public:
    Lcd_Comm(int idx);
    ~Lcd_Comm();

    void SendCommand(uint32_t cmd, uint32_t x, uint32_t y, uint32_t ex, uint32_t ey);
    uint32_t WriteAvailable();
    void WriteData(uint8_t* data, uint32_t data_size, bool flush = true);
    void SetOrientation(uint32_t orientation);
    void Reset();
    void Clear();


    uint32_t GetWidth();
    uint32_t GetHeight();
    uint32_t GetOrientation() { return m_orientation; };

private:
    uint32_t m_com_idx;
    uint32_t m_has_reset;
    uint32_t m_display_width;
    uint32_t m_display_height;
    uint32_t m_orientation;
};