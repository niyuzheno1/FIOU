cmake_minimum_required (VERSION 3.21)
project(FIOU VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(FIOU_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(FIOU_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/headers")
set(FIOU_EXPORT_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/export_headers")

# add header files from headers directory

file(GLOB_RECURSE  FIOU_SOURCE_FILES
     "${FIOU_SOURCE_DIR}/*.cpp"
     "${FIOU_HEADER_DIR}/*.h"
      "${FIOU_EXPORT_HEADER_DIR}/*.h"
     "${FIOU_SOURCE_DIR}/*.c"
     )

add_executable(FIOU ${FIOU_SOURCE_FILES})
target_include_directories(FIOU PUBLIC ${FIOU_HEADER_DIR})
target_include_directories(FIOU PUBLIC ${FIOU_EXPORT_HEADER_DIR})

