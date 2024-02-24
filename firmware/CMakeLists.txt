# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS ./components/esp-idf-lib/components/
                         ./components/esp-idf-mylib/components/)

set(CMAKE_EXPORT_COMPILE_COMMAND ON)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(main)
