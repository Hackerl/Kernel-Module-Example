# Find the kernel release
execute_process(
        COMMAND uname -r
        OUTPUT_VARIABLE KERNEL_RELEASE
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Find the headers
find_path(
        KERNEL_DIR
        modules.symbols
        PATHS /lib/modules/${KERNEL_RELEASE}
)

execute_process(
        COMMAND readlink -fn ${KERNEL_DIR}/build
        OUTPUT_VARIABLE KERNEL_HEADERS_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
        COMMAND readlink -fn ${KERNEL_DIR}/source
        OUTPUT_VARIABLE KERNEL_SOURCE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "Kernel release: ${KERNEL_RELEASE}")
message(STATUS "Kernel headers: ${KERNEL_HEADERS_DIR}")
message(STATUS "Kernel source: ${KERNEL_SOURCE_DIR}")

if (KERNEL_HEADERS_DIR)
    set(
            KERNEL_HEADERS_INCLUDE_DIRS
            ${KERNEL_SOURCE_DIR}/include
            ${KERNEL_HEADERS_DIR}/include
            ${KERNEL_HEADERS_DIR}/arch/x86/include
    )
    set(KERNEL_HEADERS_FOUND 1 CACHE STRING "Set to 1 if kernel headers were found")
else (KERNEL_HEADERS_DIR)
    set(KERNEL_HEADERS_FOUND 0 CACHE STRING "Set to 1 if kernel headers were found")
endif (KERNEL_HEADERS_DIR)

mark_as_advanced(KERNEL_HEADERS_FOUND)