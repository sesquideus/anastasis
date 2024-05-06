cmake_minimum_required(VERSION 3.25)
project(anastasis_cpp)

set(CMAKE_CXX_STANDARD 23)
set(EIGEN_DIR "/usr/include/eigen3/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -O2 -DNDEBUG")

find_package(fmt)
target_link_libraries(fmt::fmt)

include_directories("/home/kvik/astar/anastasis/cpp/")
include_directories(${EIGEN_DIR})
add_executable(
        anastasis_cpp
        main.cpp
        types/types.h
        types/point.h
        types/point.cpp
        utils/functions.h
        utils/functions.cpp
        #spatial/spatial.h
#        spatial/metrics.h
#        spatial/structures/vpitree.tpp
#        spatial/structures/vpitree.h
#        spatial/distant.cpp
#        spatial/distant.h
#        spatial/structures/kdtree.cpp
#        spatial/structures/kdtree.h
#        spatial/structures/balltree.cpp
#        spatial/structures/balltree.h
        grid/placedgrid.tpp
        grid/placedgrid.h
        grid/pixel/pixel.cpp
        grid/pixel/pixel.h
        grid/modelimage.cpp
        grid/modelimage.h
        grid/box.h
        grid/detectorimage.cpp
        grid/detectorimage.h
        grid/box.cpp
        grid/image.tpp
        grid/image.h
        grid/abstractgrid.h
        utils/resample.cpp
        utils/resample.h
)

add_executable(
        subpixel
        subpixel.cpp
        types/types.h
        types/point.h
        types/point.cpp
        utils/functions.h
        utils/functions.cpp
        utils/resample.h
        utils/resample.cpp
        grid/abstractgrid.h
        grid/image.h
        grid/placedgrid.h
        grid/pixel/pixel.cpp
        grid/pixel/pixel.h
        grid/modelimage.cpp
        grid/modelimage.h
        grid/detectorimage.cpp
        grid/detectorimage.h
        utils/eigen.cpp
        utils/eigen.h
)

add_executable(
        metis
        metis.cpp
        types/types.h
        types/point.h
        types/point.cpp
        utils/functions.h
        utils/functions.cpp
        utils/resample.h
        utils/resample.cpp
        grid/abstractgrid.h
        grid/image.h
        grid/placedgrid.h
        grid/pixel/pixel.cpp
        grid/pixel/pixel.h
        grid/modelimage.cpp
        grid/modelimage.h
        grid/detectorimage.cpp
        grid/detectorimage.h
        utils/eigen.cpp
        utils/eigen.h
)
