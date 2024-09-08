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

project(Test C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(Test)

target_sources(Test PRIVATE
 Test.cpp
 lcd_comm.cpp
 pico_pio_usb/pio_usb.c
 pico_pio_usb/pio_usb_device.c
 pico_pio_usb/pio_usb_host.c
 pico_pio_usb/usb_crc.c
 usb_descriptors.c
 # can use 'tinyusb_pico_pio_usb' library later when pico-sdk is updated
 ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/pio_usb/dcd_pio_usb.c
 ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/pio_usb/hcd_pio_usb.c
 )

pico_set_program_name(Test "Test")
pico_set_program_version(Test "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Test 0)
pico_enable_stdio_usb(Test 0)

# use tinyusb implementation
# target_compile_definitions(${target_name} PRIVATE PIO_USB_USE_TINYUSB)

# Add the standard include files to the build
target_include_directories(Test PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/pico_pio_usb
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add the standard library to the build
target_link_libraries(Test
        pico_stdlib pico_multicore hardware_pio hardware_dma tinyusb_device tinyusb_host)

pico_add_extra_outputs(Test)

