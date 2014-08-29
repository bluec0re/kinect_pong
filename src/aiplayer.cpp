#include "aiplayer.h"
#include "playstate.h"

#define PADDLE_SPEED 100

void AiPlayer::update(double time) {
    std::vector<BallPtr> balls = _state->getBalls();
    if(balls.size() > 0) {
        BallPtr ball = balls[0];

        Ogre::Real y = 0, z = 0;

        if(ball->getPosition().y > _paddle->getPosition().y) {
            y =  PADDLE_SPEED;
        } else if(ball->getPosition().y < _paddle->getPosition().y) {
            y = -PADDLE_SPEED;
        }


        if(ball->getPosition().z > _paddle->getPosition().z) {
            z =  PADDLE_SPEED;
        } else if(ball->getPosition().z < _paddle->getPosition().z) {
            z = -PADDLE_SPEED;
        }

        _paddle->setSpeed(Ogre::Vector3(0, y, z));
    }
}
