#include "pong3dstate.h"

Pong3DState::Pong3DState()
{
}


void Pong3DState::enter() {
    PlayState::enter();
    createBox(0.5,0.5,0.1, Ogre::Plane(Ogre::Vector3(1, 0, 0), -190));
    Ogre::Camera* cam = mDevice->sceneMgr->getCamera("PlayerCam");

    // Position it at 500 in Z direction
    cam->setPosition(Ogre::Vector3(-500, 0,0));
    // Look back along -Z
    cam->lookAt(Ogre::Vector3(300,0,0));
}


bool Pong3DState::frameRenderingQueued(const Ogre::FrameEvent& evt) {
    if(mDevice->keyboard->isKeyDown(OIS::KC_UP)) {
        _paddles[0]->setSpeed(Ogre::Vector3(0, 100, 0));
    } else if(mDevice->keyboard->isKeyDown(OIS::KC_DOWN)) {
        _paddles[0]->setSpeed(Ogre::Vector3(0, -100, 0));
    } else if(mDevice->keyboard->isKeyDown(OIS::KC_LEFT)) {
        _paddles[0]->setSpeed(Ogre::Vector3(0, 0, -100));
    } else if(mDevice->keyboard->isKeyDown(OIS::KC_RIGHT)) {
        _paddles[0]->setSpeed(Ogre::Vector3(0, 0, 100));
    } else {
        _paddles[0]->setSpeed(Ogre::Vector3::ZERO);
    }

    return PlayState::frameRenderingQueued(evt);
}
