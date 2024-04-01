# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C++ Standard
project(picow-metar C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# add a compilation subdirectory
add_subdirectory(lib/Config)
add_subdirectory(lib/e-Paper)
add_subdirectory(lib/Fonts)
add_subdirectory(lib/GUI)
add_subdirectory(lib/HTTP)
add_subdirectory(examples)

# add a header directory
include_directories(examples)
include_directories(./lib/Config)
include_directories(./lib/GUI)
include_directories(./lib/e-Paper)
include_directories(./lib/HTTP)

if (DEFINED ENV{WIFI_SSID} AND (NOT WIFI_SSID))
	set(WIFI_SSID $ENV{WIFI_SSID})
	message("Using WIFI_SSID from environment ('${WIFI_SSID}')")
endif()

if (DEFINED ENV{WIFI_PASSWORD} AND (NOT WIFI_PASSWORD))
	set(WIFI_PASSWORD $ENV{WIFI_PASSWORD})
	message("Using WIFI_PASSWORD from environment")
endif()

add_executable(${PROJECT_NAME}
        main.c
        )
target_compile_definitions(${PROJECT_NAME} PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
	)
# pull in common dependencies
target_link_libraries(${PROJECT_NAME}
		pico_stdlib
		hardware_spi
		examples
		ePaper
		GUI
		Fonts
		Config
		example_lwip_http_util
)

# create map/bin/hex file etc.
pico_add_extra_outputs(${PROJECT_NAME})

# add url via pico_set_program_url
# example_auto_set_url(${PROJECT_NAME})

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
