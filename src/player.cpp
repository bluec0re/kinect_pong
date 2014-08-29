#include "player.h"

#include "pong2dstate.h"
#include "pong3dstate.h"

bool Player::is2D() const {
    return dynamic_cast<Pong2DState*>(_state) != nullptr;
}


bool Player::is3D() const {
    return dynamic_cast<Pong3DState*>(_state) != nullptr;
}
