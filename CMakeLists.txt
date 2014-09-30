cmake_minimum_required( VERSION 2.8.1 )

project( KinectPong )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" )
set( CMAKE_BUILD_TYPE Debug )

option( ENABLE_OPENCV "Enable OpenCV Support" ON )
option( ENABLE_OPENNI "Enable OpenNI Support" ON )

find_package( Boost
                    COMPONENTS filesystem system
                    REQUIRED
)
find_package( OGRE REQUIRED )
find_package( OIS REQUIRED )
find_package( CEGUI 0.8 REQUIRED )

if( ENABLE_OPENNI )
    find_package( OpenNI2 )
endif( ENABLE_OPENNI )

if( ENABLE_OPENCV )
    find_package( OpenCV )
endif( ENABLE_OPENCV )

if(OPENNI2_FOUND)
    add_definitions(-DHAVE_OPENNI2)
endif(OPENNI2_FOUND)


if(OpenCV_FOUND)
    add_definitions(-DHAVE_OPENCV)
endif(OpenCV_FOUND)

include_directories( AFTER ${OGRE_INCLUDE_DIRS} ${OGRE_Overlay_INCLUDE_DIRS}
                           ${OIS_INCLUDE_DIR} ${CEGUI_INCLUDE_DIRS}
                           ${OPENNI2_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} )

set( NITE2_DIR "${CMAKE_SOURCE_DIR}/libs/Nite2.2" )
include_directories( AFTER "${NITE2_DIR}/Include" )
link_directories( "${NITE2_DIR}/Redist/" )


list( APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}" )

add_subdirectory( src )

if( OPENNI2_FOUND )
    add_subdirectory( kinect_sandbox )
endif()
