#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "player.h"

class AiPlayer : public Player
{
public:
    using Player::Player;

    virtual void update(double time = 0);
};

#endif // AIPLAYER_H
