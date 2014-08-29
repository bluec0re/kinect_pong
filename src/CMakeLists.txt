set( SRC
        main.cpp
        global.h
        gamestate.h
        gamestatemanager.h
        gamestatemanager.cpp
        paddle.h
        paddle.cpp
        ball.h
        ball.cpp
        playstate.h
        playstate.cpp
        object.h
        object.cpp
        pong2dstate.h
        pong2dstate.cpp
        pong3dstate.h
        pong3dstate.cpp
        player.h
        player.cpp
        keyboardplayer.h
        keyboardplayer.cpp
        kinect.h
        kinect.cpp
        kinectplayer.h
        kinectplayer.cpp
        aiplayer.h
        aiplayer.cpp
    )

set(CMAKE_CXX_FLAGS "-std=c++11" )

add_executable( ${PROJECT_NAME} ${SRC} )
target_link_libraries( ${PROJECT_NAME} ${OGRE_LIBRARIES} ${OGRE_Overlay_LIBRARIES} ${OIS_LIBRARIES} ${Boost_LIBRARIES} ${OPENNI2_LIBRARIES} NiTE2 )
