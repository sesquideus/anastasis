cmake_minimum_required(VERSION 3.26)
project(anastasis_cpp)

set(CMAKE_CXX_STANDARD 23)
set(EIGEN_DIR "/home/kvik/eigen/")

find_package(fmt)
target_link_libraries(fmt::fmt)

include_directories("/home/kvik/aster/anastasis/cpp/")
include_directories(${EIGEN_DIR})
add_executable(
        anastasis_cpp
        main.cpp
        spatial/spatial.h
        grid/grid.cpp
        grid/grid.h
        types/types.h
        types/point.h
        spatial/metrics.h
        spatial/structures/vpitree.tpp
        spatial/structures/vpitree.h
        spatial/distant.cpp
        spatial/distant.h
        spatial/structures/kdtree.cpp
        spatial/structures/kdtree.h
        spatial/structures/balltree.cpp
        spatial/structures/balltree.h
        grid/pixel.cpp
        grid/pixel.h
        grid/canonicalgrid.cpp
        grid/canonicalgrid.h
        types/point.cpp
)
