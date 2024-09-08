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


private:
    uint32_t com_idx;
    uint32_t display_width;
    uint32_t display_height;
};