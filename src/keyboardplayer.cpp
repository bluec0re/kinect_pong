#include "keyboardplayer.h"
#include "player.h"
#include "paddle.h"
#include "playstate.h"

#define PADDLE_SPEED 200

KeyboardPlayer::KeyboardPlayer(const Ogre::String& name, Paddle *paddle, PlayState *state)
        : Player(name + Ogre::String(" (Keyboard)"), paddle, state) {
}

void KeyboardPlayer::update(double timeSinceLastFrame) {
    Ogre::Real y = 0, z = 0;

    if(_state->getDevices()->keyboard->isKeyDown(OIS::KC_UP)) {
        y =  PADDLE_SPEED;
    } else if(_state->getDevices()->keyboard->isKeyDown(OIS::KC_DOWN)) {
        y = -PADDLE_SPEED;
    }

    if(is3D()) {
        if(_state->getDevices()->keyboard->isKeyDown(OIS::KC_LEFT)) {
            z = -PADDLE_SPEED;
        } else if(_state->getDevices()->keyboard->isKeyDown(OIS::KC_RIGHT)) {
            z =  PADDLE_SPEED;
        }
    }

    _paddle->setSpeed(Ogre::Vector3(0, y, z));
}
