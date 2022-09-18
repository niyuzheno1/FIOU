cmake_minimum_required (VERSION 3.16)
project(FIOU_Library VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
include(FetchContent)

FetchContent_Declare(
    MMS
    GIT_REPOSITORY https://github.com/niyuzheno1/MMS.git
    GIT_TAG        8f813bc4301449b2fcdb81ba14b181211827e230 # release-1.0.0
)

FetchContent_MakeAvailable(MMS)



file(GLOB_RECURSE  FIOU_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/headers/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/headers/*.c
     )
    



add_library(FIOU_Library ${FIOU_SOURCE_FILES})
target_include_directories(FIOU_Library PUBLIC ${MMS_SOURCE_DIR}/export_headers)
target_link_libraries(FIOU_Library MMS_Library)
target_include_directories(FIOU_Library  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/headers)

# install(TARGETS FIOU_Library DESTINATION lib)
# install(FILES ???.h DESTINATION include)