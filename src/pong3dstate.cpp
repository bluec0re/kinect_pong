#include "pong3dstate.h"

void Pong3DState::enter() {
    PlayState::enter();
    createBox(0.5,0.5,0.1, Ogre::Plane(Ogre::Vector3(1, 0, 0), -390));
    Ogre::Camera* cam = mDevice->sceneMgr->getCamera("PlayerCam");

    // Position it at 500 in Z direction
    cam->setPosition(Ogre::Vector3(-900, 0,0));
    // Look back along -Z
    cam->lookAt(Ogre::Vector3(300,0,0));
}


PaddlePtr Pong3DState::addPaddle(int color, const Ogre::String& name, const Ogre::Real& pos) {
    // is player 1
    bool transparent = pos < 0;

    PaddlePtr p(new Paddle(color, name, transparent));
    p->setSceneManager(mDevice->sceneMgr);
    p->create();
    p->setPosition(Ogre::Vector3(pos, 0, 0));
    _paddles.push_back(p);

    return p;
}


BallPtr Pong3DState::addBall() {
    BallPtr ball(new Ball(0xFF00FF00, 0xFFBF6900, true));
    ball->setSceneManager(mDevice->sceneMgr);
    ball->create();
    ball->setAccel(getRandomAccel());
    ball->enableAccel(false);
    ball->setSpeed(getRandomSpeed());
    _balls.push_back(ball);

    return ball;
}

