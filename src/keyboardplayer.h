#ifndef KEYBOARDPLAYER_H
#define KEYBOARDPLAYER_H

#include "player.h"

class KeyboardPlayer : public Player
{
public:
    KeyboardPlayer(const Ogre::String& name, const PaddlePtr& paddle, PlayState *state);

    virtual ~KeyboardPlayer() {}

    virtual void update(double timeSinceLastFrame = 0) override;
};

#endif // KEYBOARDPLAYER_H
