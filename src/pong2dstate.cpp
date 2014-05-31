#include "pong2dstate.h"

Pong2DState::Pong2DState()
{
}


bool Pong2DState::frameRenderingQueued(const Ogre::FrameEvent& evt) {
    if(mDevice->keyboard->isKeyDown(OIS::KC_UP)) {
        _paddles[0]->setSpeed(Ogre::Vector3(0, 100, 0));
    } else if(mDevice->keyboard->isKeyDown(OIS::KC_DOWN)) {
        _paddles[0]->setSpeed(Ogre::Vector3(0, -100, 0));
    } else {
        _paddles[0]->setSpeed(Ogre::Vector3::ZERO);
    }

    return PlayState::frameRenderingQueued(evt);
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
