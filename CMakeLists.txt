cmake_minimum_required(VERSION 3.12)

set(NAME picovision-pianovision)  # TODO: Change me!

include(pimoroni_pico_import.cmake)
include(pico_sdk_import.cmake)

# Gooey pianovision
project(${NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

include(picovision_import.cmake)

##
include(pimoroni_pico_import.cmake)
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)
##
# Include required libraries
# This assumes `pimoroni-pico` is stored alongside your project

# I2C and SPI
include(common/pimoroni_i2c)
include(common/pimoroni_bus)

# Graphics stuff
include(libraries/jpegdec/jpegdec)
include(libraries/pico_graphics/pico_graphics)

# SD Card Access
include(drivers/fatfs/fatfs)
include(drivers/sdcard/sdcard)

# Add your source files
add_executable(${NAME}
   main.cpp # TODO: Add source files here!
   #main-demo.cpp # TODO: Add source files here!
)

# TODO: Don't forget to link the libraries you need!
target_link_libraries(${NAME}
pico_stdlib
    picovision
    pico_graphics
    jpegdec
    pimoroni_i2c
    fatfs
    sdcard
)

target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(${NAME} PRIVATE
    USB_HOST=1
    SDCARD_SPI_BUS=spi1
    SDCARD_PIN_SPI0_CS=15
    SDCARD_PIN_SPI0_SCK=10
    SDCARD_PIN_SPI0_MOSI=11
    SDCARD_PIN_SPI0_MISO=12
)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

pico_enable_stdio_uart(${NAME} 1)
pico_enable_stdio_usb(${NAME} 0)

target_link_options(${NAME} PRIVATE
    -Wl,--print-memory-usage
)

# Set up files for the release packages
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.uf2
    ${CMAKE_CURRENT_LIST_DIR}/README.md
    DESTINATION .
)

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_GENERATOR "ZIP" "TGZ")
include(CPack)
