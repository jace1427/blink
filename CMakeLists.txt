cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(test test.c)

pico_enable_stdio_usb(test 1)
pico_enable_stdio_uart(test 0)

target_link_libraries(test pico_stdlib pico_cyw43_arch_none hardware_i2c)

pico_add_extra_outputs(test)

