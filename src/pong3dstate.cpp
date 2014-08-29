#include "pong3dstate.h"

Pong3DState::Pong3DState()
{
}


void Pong3DState::enter() {
    PlayState::enter();
    createBox(0.5,0.5,0.1, Ogre::Plane(Ogre::Vector3(1, 0, 0), -190));
    Ogre::Camera* cam = mDevice->sceneMgr->getCamera("PlayerCam");

    // Position it at 500 in Z direction
    cam->setPosition(Ogre::Vector3(-600, 0,0));
    // Look back along -Z
    cam->lookAt(Ogre::Vector3(300,0,0));
}
