#ifndef GUISTATE_H
#define GUISTATE_H

#include "gamestate.h"

class GuiState : public GameState
{
public:
    using GameState::GameState;

    virtual bool keyPressed(const OIS::KeyEvent &arg);
    virtual bool keyReleased(const OIS::KeyEvent &arg);
    virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
};

#endif // GUISTATE_H
