#include <stdio.h>
#include "lcd/lcd_comm.h"
#include "lcd/image_handler.h"
#include "img_data/climb.h"
#include "img_data/climb_2.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/divider.h"
#include "hardware/clocks.h"
#include "pico/bootrom.h"
#include "pico_pio_usb/pio_usb.h"
#include "tusb.h"

#define PIO_USB_USE_TINYUSB

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define BAUD_RATE 9600

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 4
#define UART_RX_PIN 5

// GPIO defines
// Example uses GPIO 2
#define R_LED 17
#define R_LED_MASK ~(1 << (R_LED))
#define G_LED 16
#define G_LED_MASK ~(1 << (G_LED))
#define B_LED 25
#define B_LED_MASK ~(1 << (B_LED))

#define RGB_MASK (1 << R_LED | 1 << G_LED | 1 << B_LED)

void cdc_app_task(void);

static usb_device_t *usb_device = NULL;
static Lcd_Comm *screen = NULL;
static Image_Handler *climb_image = NULL;
static Image_Handler *climb_2_image = NULL;
static uint32_t connected = 0;
static uint8_t cleared = 0;
void core1_main() {
    sleep_ms(10);

    // To run USB SOF interrupt in core1, create alarm pool in core1.
    static pio_usb_configuration_t config = PIO_USB_DEFAULT_CONFIG;
    config.alarm_pool = (void*)alarm_pool_create(2, 1);
    config.pin_dp = 3;
    tuh_configure(1, TUH_CFGID_RPI_PIO_USB_CONFIGURATION, &config);

    tuh_init(1);
    while (true) {
        tuh_task();
        cdc_app_task();
    }
}

void heartbeat() 
{
    static absolute_time_t timeout = make_timeout_time_ms(1000);
    static uint32_t ON_LED = R_LED_MASK;
    static uint32_t NEXT_LED = B_LED_MASK;
    absolute_time_t current_time = get_absolute_time();
    if (current_time >= timeout) {
        uint32_t temp = ON_LED;
        gpio_clr_mask(RGB_MASK);
        gpio_set_mask(ON_LED);
        ON_LED = NEXT_LED;
        NEXT_LED = temp;
        timeout = make_timeout_time_ms(1000);
        char tempbuf[256];
        int count = sprintf(tempbuf, "Alive, Connected: %d, Buffer Size: %d\r\n", connected, screen->WriteAvailable());
        tud_cdc_write(tempbuf, count);
    }
}

int main()
{
    set_sys_clock_khz(120000, true);
    // stdio_init_all();

    multicore_reset_core1();
    // all USB task run in core1
    multicore_launch_core1(core1_main);
    gpio_init(R_LED);
    gpio_set_dir(R_LED, GPIO_OUT);
    gpio_init(G_LED);
    gpio_set_dir(G_LED, GPIO_OUT);
    gpio_init(B_LED);
    gpio_set_dir(B_LED, GPIO_OUT);
    gpio_clr_mask(RGB_MASK);
    // init device stack on native usb (roothub port0)
    tud_init(0);

    while (true) {
        tud_task();
        tud_cdc_write_flush();
        heartbeat();
    }
}

//--------------------------------------------------------------------+
// Device CDC
//--------------------------------------------------------------------+

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf)
{
  (void) itf;

  char buf[64];
  uint32_t count = tud_cdc_read(buf, sizeof(buf));

  // TODO control LED on keyboard of host stack
  (void) count;
}

//--------------------------------------------------------------------+
// Host CDC
//--------------------------------------------------------------------+

void cdc_app_task(void) {
    bool toggle = true;
    static absolute_time_t timeout = make_timeout_time_ms(10000);
    absolute_time_t current_time = get_absolute_time();
    if (current_time >= timeout) {
        timeout = make_timeout_time_ms(10000);
        toggle = !toggle;
    }

    if (screen != NULL && climb_image != NULL && toggle) {
        
        climb_2_image->DisplayImage();
    }

    if (screen != NULL && climb_2_image != NULL && !toggle) {
        
        climb_image->DisplayImage();
    }
}

// Invoked when received new data
void tuh_cdc_rx_cb(uint8_t idx) {
    uint8_t buf[64 + 1]; // +1 for extra null character
    char tempbuf[256];
    uint32_t const bufsize = sizeof(buf) - 1;

    // forward cdc interfaces -> console
    uint32_t count = tuh_cdc_read(idx, buf, bufsize);
    buf[count] = 0;

    count = sprintf(tempbuf, "%s", (char*) buf);
    tud_cdc_write(tempbuf, count);
    tud_cdc_write_flush();
}

void tuh_cdc_mount_cb(uint8_t idx)
{
    char tempbuf[256];
    int count = 0;
    tuh_itf_info_t itf_info = {0};
    tuh_cdc_itf_get_info(idx, &itf_info);
    connected |= 1 << idx;
    count = sprintf(tempbuf,"CDC Interface is mounted: address = %u, itf_num = %u\r\n", itf_info.daddr,
         itf_info.desc.bInterfaceNumber);
    tud_cdc_write(tempbuf, count);
    tud_cdc_write_flush();
    // while eneumerating new cdc device
    cdc_line_coding_t line_coding = {0};
    if (tuh_cdc_get_local_line_coding(idx, &line_coding)) {
        count = sprintf(tempbuf,"  Baudrate: %" PRIu32 ", Stop Bits : %u\r\n", line_coding.bit_rate, line_coding.stop_bits);
        tud_cdc_write(tempbuf, count);
        count = sprintf(tempbuf,"  Parity  : %u, Data Width: %u\r\n", line_coding.parity, line_coding.data_bits);
        tud_cdc_write(tempbuf, count);
    }

    screen = new Lcd_Comm(idx);
    climb_image = new Image_Handler(screen,climb,climb_width,climb_height,climb_size);
    climb_2_image = new Image_Handler(screen,climb_2,climb_2_width,climb_2_height,climb_2_size);
}

// Invoked when device with hid interface is un-mounted
void tuh_cdc_umount_cb(uint8_t idx)
{
    char tempbuf[256];
    int count = sprintf(tempbuf, "Unmounted IDX: %d \r\n", idx);
    delete screen;
    screen = NULL;
    tud_cdc_write(tempbuf, count);
    tud_cdc_write_flush();
}
