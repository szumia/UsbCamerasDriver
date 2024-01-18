cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} third_party_src)

add_library(third_party OBJECT ${third_party_src})
#target_link_libraries(third_party PUBLIC )