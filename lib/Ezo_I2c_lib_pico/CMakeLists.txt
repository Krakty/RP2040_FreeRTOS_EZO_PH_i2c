cmake_minimum_required(VERSION 3.12)

# Set the project name and languages
project(Ezo_I2c_lib_pico C CXX ASM)

# Import the Pico SDK (assumes the PICO_SDK_PATH environment variable is set)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Initialize the Pico SDK
pico_sdk_init()

# Create a static library from the source files
add_library(ezo_i2c_lib
    src/Ezo_i2c.cpp
    src/Ezo_i2c_util.cpp
    src/iot_cmd.cpp
    src/sequencer1.cpp
    src/sequencer2.cpp
    src/sequencer3.cpp
    src/sequencer4.cpp
)

# Specify the include directory for header files
target_include_directories(ezo_i2c_lib PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Link against Pico libraries as needed
target_link_libraries(ezo_i2c_lib PUBLIC pico_stdlib hardware_i2c)

# Optionally, if you want to generate additional output files for debugging/library analysis:
# pico_add_extra_outputs(ezo_i2c_lib)
