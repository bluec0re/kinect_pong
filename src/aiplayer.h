#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "player.h"

class AiPlayer : public Player
{
public:
    enum Strength {
        AI_WEAK,
        AI_STRONG
    };

    AiPlayer(const Strength& s, const Ogre::String& name, Paddle* paddle, PlayState* state);

    virtual void update(double time = 0) override;

private:
    Strength _strength;
};

#endif // AIPLAYER_H
