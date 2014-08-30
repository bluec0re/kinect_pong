#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "gamestate.h"

class MenuState : public GameState
{
public:
    using GameState::GameState;


    virtual void enter();
    virtual void exit();
    virtual bool pause() { return true; }
    virtual void resume() {}

    virtual bool keyPressed(const OIS::KeyEvent &arg);
    virtual bool keyReleased(const OIS::KeyEvent &arg);
    virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
};

#endif // MENUSTATE_H
