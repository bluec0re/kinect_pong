set( SRC
        handtracker.cpp
        NiteSampleUtilities.h
    )

set(CMAKE_CXX_FLAGS "-std=c++11" )

add_executable( handtracker ${SRC} )
target_link_libraries( handtracker ${OPENNI2_LIBRARIES} NiTE2 )

set( SRC
        usertracker.cpp
        NiteSampleUtilities.h
    )

add_executable( usertracker ${SRC} )
target_link_libraries( usertracker ${OPENNI2_LIBRARIES} NiTE2 )

add_custom_command(TARGET usertracker POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                  ${NITE2_DIR}/Redist/ $<TARGET_FILE_DIR:usertracker>)
