#ifndef PON32DSTATE_H
#define PON32DSTATE_H

#include "playstate.h"

class Pong3DState : public PlayState
{
public:
    using PlayState::PlayState;

    DECLARE_GAMESTATE_CLASS(Pong3DState)

    virtual void enter() override;
    virtual PaddlePtr addPaddle(int color, const Ogre::String& name, const Ogre::Real& pos) override;
};

#endif // PON32DSTATE_H
