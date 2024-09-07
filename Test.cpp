#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/divider.h"
#include "hardware/clocks.h"
#include "pico/bootrom.h"
#include "pico_pio_usb/pio_usb.h"
#include "tusb.h"



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
#define R_LED_MASK 1 << (R_LED)
#define G_LED 16
#define G_LED_MASK 1 << (G_LED)
#define B_LED 25
#define B_LED_MASK 1 << (B_LED)

#define RGB_MASK (1 << R_LED | 1 << G_LED | 1 << B_LED)

static usb_device_t *usb_device = NULL;
void core1_main() {
  sleep_ms(10);

  // To run USB SOF interrupt in core1, create alarm pool in core1.
  static pio_usb_configuration_t config = PIO_USB_DEFAULT_CONFIG;
  config.pin_dp = 3;
  config.alarm_pool = (void*)alarm_pool_create(2, 1);
  usb_device = pio_usb_host_init(&config);

  //// Call pio_usb_host_add_port to use multi port
  // const uint8_t pin_dp2 = 8;
  // pio_usb_host_add_port(pin_dp2);

  while (true) {
    pio_usb_host_task();
  }
}


int main()
{
    set_sys_clock_khz(120000, true);
    stdio_init_all();

    multicore_reset_core1();
    // all USB task run in core1
    multicore_launch_core1(core1_main);

    // init device stack on native usb (roothub port0)
    tud_init(0);
    
    gpio_init(R_LED);
    gpio_set_dir(R_LED, GPIO_OUT);

    gpio_init(G_LED);
    gpio_set_dir(G_LED, GPIO_OUT);

    gpio_init(B_LED);
    gpio_set_dir(B_LED, GPIO_OUT);

    while (true) {
        tud_task();
        gpio_clr_mask(RGB_MASK);
        gpio_set_mask(R_LED_MASK);
        sleep_ms(333);
        gpio_clr_mask(RGB_MASK);
        gpio_set_mask(G_LED_MASK);
        sleep_ms(333);
        gpio_clr_mask(RGB_MASK);
        gpio_set_mask(B_LED_MASK);
        sleep_ms(333);

        if (usb_device != NULL) {
            for (int dev_idx = 0; dev_idx < PIO_USB_DEVICE_CNT; dev_idx++) {
                usb_device_t *device = &usb_device[dev_idx];
                if (!device->connected) {
                    continue;
                }

                // Print received packet to EPs
                for (int ep_idx = 0; ep_idx < PIO_USB_DEV_EP_CNT; ep_idx++) {
                    endpoint_t *ep = pio_usb_get_endpoint(device, ep_idx);

                    if (ep == NULL) {
                        printf("Endpoint %d, Null, Dev %d\n", ep_idx, dev_idx);
                        break;
                    }

                    uint8_t temp[64];
                    int len = pio_usb_get_in_data(ep, temp, sizeof(temp));

                    if (len > 0) {
                        printf("%04x:%04x device class:%04x, EP 0x%02x:\t", device->vid, device->pid, device->device_class,
                            ep->ep_num);
                        for (int i = 0; i < len; i++) {
                        printf("%02x ", temp[i]);
                        }
                        printf("\n");
                    } else {
                        printf("No Data\n");
                    }
                }
            }
        }
    }
}



