cmake_minimum_required(VERSION 3.14)
project(Projet_OpenCV_CMake)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})


add_executable(Projet_OpenCV_CMake
        include/histogram.hpp
        include/binary_code.hpp
        include/zoom.hpp
        include/Label_recognition.h
        src/histogram.cpp
        src/binary_code.cpp
        src/zoom.cpp
        #src/edge_detection_cany.cpp
        #src/Morphology.cpp
        #src/MatchTemplate_Demo.cpp
        include/affichage.h
        include/retriveSymbol.h
        src/histogram.cpp
        src/affichage.cpp
        src/retriveSymbol.cpp
        src/warpping.cpp
        src/main.cpp
        src/Label_recognition.cpp
)
# src/main_test_opencv.cpp

target_link_libraries(Projet_OpenCV_CMake ${OpenCV_LIBS})

