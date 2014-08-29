#ifndef PONG2DSTATE_H
#define PONG2DSTATE_H

#include "playstate.h"

class Pong2DState : public PlayState
{
public:
    Pong2DState();
    DECLARE_GAMESTATE_CLASS(Pong2DState)

    virtual void enter();
    void resetBall(BallPtr b);

protected:
    virtual Ogre::Vector3 getRandomAccel() const;
};

#endif // PONG2DSTATE_H
