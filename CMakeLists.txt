cmake_minimum_required(VERSION 3.0)
project(DriverDemo C)

set(CMAKE_C_STANDARD 99)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(KernelHeaders REQUIRED)
include_directories(${KERNEL_HEADERS_INCLUDE_DIRS})

set(DRIVER_FILE main.ko)
set(SOURCE_FILES main.c)
set(OBJECT_FILES main.o)

set(KERNEL_BUILD_CMD $(MAKE) -C ${KERNEL_HEADERS_DIR} modules M=${CMAKE_CURRENT_BINARY_DIR} src=${CMAKE_CURRENT_SOURCE_DIR})

FILE(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/Kbuild "obj-m := ${OBJECT_FILES}")

add_custom_command(OUTPUT ${DRIVER_FILE}
        COMMAND ${KERNEL_BUILD_CMD}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${SOURCE_FILES} VERBATIM)

add_custom_target(driver ALL DEPENDS ${DRIVER_FILE})

# puppet target for clion parse symbols
# add_library(puppet STATIC ${SOURCE_FILES})