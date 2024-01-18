cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} process_img_src)

add_library(process_img OBJECT ${process_img_src})

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
target_link_libraries(process_img ${OpenCV_LIBS})
