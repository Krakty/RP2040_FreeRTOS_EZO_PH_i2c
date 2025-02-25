# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD rp2040_PURPLE_USBC_16M_NONRGB CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)

if(DEFINED ENV{PICO_SDK_PATH})
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
    message("Using PICO_SDK_PATH from environment ('${PICO_SDK_PATH}')")
else()
    message(FATAL_ERROR "PICO_SDK_PATH is not defined. Please set it in your environment (e.g., .zshrc) to /home/tlindell/.pico-sdk/sdk/2.1.1")
endif()
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

if(DEFINED ENV{FREERTOS_KERNEL_PATH})
    set(FREERTOS_KERNEL_PATH $ENV{FREERTOS_KERNEL_PATH})
    message("Using FREERTOS_KERNEL_PATH from environment ('${FREERTOS_KERNEL_PATH}')")
else()
    message(FATAL_ERROR "FREERTOS_KERNEL_PATH is not defined. Please set it in your environment (e.g., .zshrc) to /home/tlindell/WORK/EMBEDDED/SDKs/FreeRTOS/FreeRTOS/Source")
endif()
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(RP2040_FRTOS_EZO_PH_i2c C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# Add Ezo_I2c_lib_pico library
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/Ezo_I2c_lib_pico)

# Ensure FreeRTOS-Kernel has access to necessary Pico SDK headers (use INTERFACE scope)
target_include_directories(FreeRTOS-Kernel INTERFACE
    ${PICO_SDK_PATH}/src/rp2_common/hardware_sync/include
    ${PICO_SDK_PATH}/src/rp2_common/hardware_sio/include
)
target_link_libraries(FreeRTOS-Kernel INTERFACE pico_stdlib)

add_executable(RP2040_FRTOS_EZO_PH_i2c
    src/main.cpp
)

target_include_directories(RP2040_FRTOS_EZO_PH_i2c PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(RP2040_FRTOS_EZO_PH_i2c PRIVATE
    pico_stdlib
    pico_stdio_uart
    pico_multicore
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap1
    ezo_i2c_lib
)

# Enable UART and USB stdio output
pico_enable_stdio_uart(RP2040_FRTOS_EZO_PH_i2c 1)
pico_enable_stdio_usb(RP2040_FRTOS_EZO_PH_i2c 1)

pico_add_extra_outputs(RP2040_FRTOS_EZO_PH_i2c)