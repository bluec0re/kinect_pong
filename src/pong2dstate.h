#ifndef PONG2DSTATE_H
#define PONG2DSTATE_H

#include "playstate.h"

class Pong2DState : public PlayState
{
public:
    DECLARE_GAMESTATE_CLASS(Pong2DState)

    virtual void enter() override;
protected:
    virtual Ogre::Vector3 getRandomSpeed() const override;
    virtual Ogre::Vector3 getRandomAccel() const override;

protected:
    using PlayState::PlayState;
};

#endif // PONG2DSTATE_H
