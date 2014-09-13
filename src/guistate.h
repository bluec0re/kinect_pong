#ifndef GUISTATE_H
#define GUISTATE_H

#include "gamestate.h"

class GuiState : public GameState
{
public:
    virtual bool keyPressed(const OIS::KeyEvent &arg) override;
    virtual bool keyReleased(const OIS::KeyEvent &arg) override;
    virtual bool mouseMoved( const OIS::MouseEvent &arg ) override;
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) override;
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) override;

    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;

protected:
    virtual void clearAll();

private:
    using GameState::GameState;
};

#endif // GUISTATE_H
