cmake_minimum_required(VERSION 3.16)

project(PlayUsbCameras)

set(CMAKE_CXX_STANDARD 17)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )


aux_source_directory("./" PROJECT_SOURCES)
add_executable(PlayUsbCameras ${PROJECT_SOURCES})

target_link_libraries(PlayUsbCameras ${OpenCV_LIBS})


include_directories(third_party)
add_subdirectory(third_party)
target_link_libraries(PlayUsbCameras third_party)

include_directories(process_img)
add_subdirectory(process_img)
target_link_libraries(PlayUsbCameras process_img)

