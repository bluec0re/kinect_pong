#include "pong2dstate.h"

Pong2DState::Pong2DState()
{
}


void Pong2DState::enter() {
    PlayState::enter();
    createBox(0.5,0.5,0.1, Ogre::Plane(Ogre::Vector3(0, 0, -1), -190));
    Ogre::Camera* cam = mDevice->sceneMgr->getCamera("PlayerCam");
    // Position it at 500 in Z direction
    cam->setPosition(Ogre::Vector3(0,0,500));
    // Look back along -Z
    cam->lookAt(Ogre::Vector3(0,0,-300));
}

void Pong2DState::resetBall(BallPtr b) {
    if(_balls.size() != 1) {
        _balls.erase(std::find(_balls.begin(), _balls.end(), b));
    } else {
        b->setPosition(Ogre::Vector3(0, 0, 0));
        Ogre::Vector3 speed;
        speed.x = getRandomSpeed();
        speed.y = getRandomSpeed();
        b->setSpeed(speed);
        b->setAccel(getRandomAccel());
    }
}


Ogre::Vector3 Pong2DState::getRandomAccel() const {
    Ogre::Vector3 accel = PlayState::getRandomAccel();
    accel.z = 0;
    return accel;
}
