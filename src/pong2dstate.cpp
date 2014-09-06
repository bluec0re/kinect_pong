#include "pong2dstate.h"

void Pong2DState::enter() {
    PlayState::enter();
    createBox(0.5,0.5,0.1, Ogre::Plane(Ogre::Vector3(0, 0, -1), -190));
    Ogre::Camera* cam = mDevice->sceneMgr->getCamera("PlayerCam");
    // Position it at 500 in Z direction
    cam->setPosition(Ogre::Vector3(0, 0, 900));
    // Look back along -Z
    cam->lookAt(Ogre::Vector3(0, 0, -300));
}

Ogre::Vector3 Pong2DState::getRandomSpeed() const {
    auto speed =  Ogre::Vector3(Ogre::Math::RangeRandom(25, 75) * (Ogre::Math::UnitRandom() > 0.5 ? -1 : 1));
    speed.z = 0;
    return speed;
}

Ogre::Vector3 Pong2DState::getRandomAccel() const {
    Ogre::Vector3 accel = PlayState::getRandomAccel();
    accel.z = 0;
    return accel;
}
