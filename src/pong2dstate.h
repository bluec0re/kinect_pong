#ifndef PONG2DSTATE_H
#define PONG2DSTATE_H

#include "playstate.h"

class Pong2DState : public PlayState
{
public:
    Pong2DState();
    DECLARE_GAMESTATE_CLASS(Pong2DState)

    virtual void enter();
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
};

#endif // PONG2DSTATE_H
