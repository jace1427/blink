cmake_minimum_required(VERSION 3.13)

# Project name
project(pico_baremetal_led C CXX ASM)

# Specify the target CPU for the Pico
set(CMAKE_SYSTEM_PROCESSOR cortex-m0plus)

# Add the main source file
add_executable(pico_baremetal_led main.c)

# Specify compiler options
target_compile_options(pico_baremetal_led PRIVATE
    -mcpu=cortex-m0plus
    -mthumb
    -Wall
    -Wextra
    -Werror
)

# Specify linker options
target_link_options(pico_baremetal_led PRIVATE
    -T ${CMAKE_SOURCE_DIR}/memmap.ld
    -nostartfiles
    -nodefaultlibs
    -nostdlib
    -Wl,--gc-sections
    -mcpu=cortex-m0plus
    -mthumb
)
