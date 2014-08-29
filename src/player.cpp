#include "player.h"

#include "pong2dstate.h"
#include "pong3dstate.h"

bool Player::is2D() const {
    return dynamic_cast<Pong2DState*>(_state) != nullptr;
}


bool Player::is3D() const {
    return dynamic_cast<Pong3DState*>(_state) != nullptr;
}


Player::Side Player::getSide() const {
    if(!_paddle)
        return SIDE_UNKNOWN;
    if(_paddle->getPosition().x < 0)
        return SIDE_LEFT;
    return SIDE_RIGHT;
}

void Player::hit() {
    _lives--;
    Ogre::StringStream ss;
    ss << "Player " << getName() << " hit. ";
    ss << _lives << " lives remaining.";
    Ogre::LogManager::getSingleton().logMessage(ss.str());
}
