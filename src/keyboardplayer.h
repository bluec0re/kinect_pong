#ifndef KEYBOARDPLAYER_H
#define KEYBOARDPLAYER_H

#include "player.h"

class KeyboardPlayer : public Player
{
public:
    using Player::Player;
    virtual ~KeyboardPlayer() {}

    virtual void update(double timeSinceLastFrame = 0);
};

#endif // KEYBOARDPLAYER_H
