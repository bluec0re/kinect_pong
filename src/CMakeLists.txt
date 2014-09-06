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
        aiplayer.h
        aiplayer.cpp
        guistate.h
        guistate.cpp
        menustate.h
        menustate.cpp
        cegui_utils.h
        cegui_utils.cpp
    )

if( ${OPENNI2_FOUND} )
    set( SRC
        ${SRC}
        kinect.h
        kinect.cpp
        kinectplayer.h
        kinectplayer.cpp
    )
endif()

if( ${OpenCV_FOUND} )
    set( SRC
        ${SRC}
        videorecorder.h
    )
endif()

add_executable( ${PROJECT_NAME} ${SRC} )
target_link_libraries( ${PROJECT_NAME} ${OGRE_LIBRARIES}
                                       ${OGRE_Overlay_LIBRARIES}
                                       ${OIS_LIBRARIES} ${CEGUI_LIBRARIES}
                                       ${CEGUI_OgreRenderer_LIBRARIES}
                                       ${Boost_LIBRARIES}
                                       ${OpenCV_LIBRARIES})

if( ${OPENNI2_FOUND} )
    target_link_libraries( ${PROJECT_NAME} ${OPENNI2_LIBRARIES} NiTE2 )
endif()
