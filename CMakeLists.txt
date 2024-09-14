# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
include(${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
# ====================================================================================
set(PICO_BOARD seeed_xiao_rp2040 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(screen_control C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(screen_control)

target_sources(screen_control PRIVATE
 main.cpp
 lcd/lcd_comm.cpp
 lcd/image_handler.cpp
 usb/pico_pio_usb/pio_usb.c
 usb/pico_pio_usb/pio_usb_device.c
 usb/pico_pio_usb/pio_usb_host.c
 usb/pico_pio_usb/usb_crc.c
 usb/usb_descriptors.c
 # can use 'tinyusb_pico_pio_usb' library later when pico-sdk is updated
 ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/pio_usb/dcd_pio_usb.c
 ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/pio_usb/hcd_pio_usb.c
 )

pico_set_program_name(screen_control "screen_control")
pico_set_program_version(screen_control "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(screen_control 0)
pico_enable_stdio_usb(screen_control 0)

# use tinyusb implementation
# target_compile_definitions(${target_name} PRIVATE PIO_USB_USE_TINYUSB)

# Add the standard include files to the build
target_include_directories(screen_control PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/usb/pico_pio_usb
  ${CMAKE_CURRENT_LIST_DIR}/usb
  ${CMAKE_CURRENT_LIST_DIR}/lcd
  ${CMAKE_CURRENT_LIST_DIR}/img_data
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add the standard library to the build
target_link_libraries(screen_control
        pico_stdlib pico_multicore pico_mem_ops hardware_pio hardware_dma tinyusb_device tinyusb_host)

pico_add_extra_outputs(screen_control)

