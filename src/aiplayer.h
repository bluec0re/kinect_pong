#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "player.h"

class AiPlayer : public Player
{
public:
    AiPlayer(const AiStrength& s, const Ogre::String& name, const PaddlePtr& paddle, PlayState* state);

    virtual void update(double time = 0) override;

private:
    AiStrength _strength;
};

#endif // AIPLAYER_H
