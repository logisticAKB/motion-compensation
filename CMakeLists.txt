cmake_minimum_required(VERSION 3.20)
project(motion_compensation)

set(CMAKE_CXX_STANDARD 11)

find_package( Threads )

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(motion_compensation main.cpp MotionCompensation.cpp MotionCompensation.h Frame.cpp Frame.h ThreadPool.cpp ThreadPool.h)
target_link_libraries(motion_compensation ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT})
#target_link_libraries(motion_compensation ${CMAKE_THREAD_LIBS_INIT})
