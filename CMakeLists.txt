cmake_minimum_required( VERSION 2.8.1 )

project( KinectPong )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" )
set( CMAKE_BUILD_TYPE Debug )

find_package( Boost
                    COMPONENTS filesystem system
                    REQUIRED
)
find_package( OGRE REQUIRED )
find_package( OIS REQUIRED )
#find_package( CEGUI REQUIRED )
find_package( OpenNI2 )

include_directories( AFTER ${OGRE_INCLUDE_DIRS} ${OGRE_Overlay_INCLUDE_DIRS} ${OIS_INCLUDE_DIR} ${OPENNI2_INCLUDE_DIRS} )

include_directories( AFTER "${CMAKE_SOURCE_DIR}/libs/Nite2.2/Include" )
link_directories( "${CMAKE_SOURCE_DIR}/libs/Nite2.2/Redist/" )

add_subdirectory( src )
