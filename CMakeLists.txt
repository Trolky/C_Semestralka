cmake_minimum_required(VERSION 3.5)

project(stegim.exe LANGUAGES C)
set(CMAKE_C_FLAGS "-lpng -Wall -Wextra -pedantic -ansi -g")


add_executable(${PROJECT_NAME} 
    src/lzw.c
    src/main.c
    src/dictionary.c
    src/file.c
    src/bmp.c
    src/steganography.c
    src/pngImage.c
)

target_include_directories(${PROJECT_NAME} PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/../../../usr/include/libpng16)

target_link_libraries(${PROJECT_NAME} PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/../../../usr/lib/x86_64-linux-gnu/libpng.a ${CMAKE_CURRENT_SOURCE_DIR}/../../../usr/lib/x86_64-linux-gnu/libz.a m)