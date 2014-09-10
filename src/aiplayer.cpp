#include "aiplayer.h"
#include "playstate.h"

#define PADDLE_SPEED 100

AiPlayer::AiPlayer(const Strength& s, const Ogre::String& name, Paddle* paddle, PlayState* state) :
    Player(name + Ogre::String(" (AI)"), paddle, state), _strength(s)
{

}

void AiPlayer::update(double time) {
    std::vector<BallPtr> balls = _state->getBalls();
    BallPtr ball(nullptr);
    auto ppos = _paddle->getPosition();
    Ogre::Real tracking_start = 0;

    switch(_strength) {
    case AI_WEAK: {
            if(balls.size() > 0)
                ball = balls[0];
        } break;

    case AI_STRONG: {
            for(BallPtr b : balls) {
                if(!ball || fabs(b->getPosition().x - ppos.x) < fabs(ball->getPosition().x - ppos.x))
                    ball = b;
            }
            tracking_start = 10000;
        } break;
    }

    Ogre::Real y = 0, z = 0;

    if(ball && (getSide() == SIDE_LEFT ? ball->getPosition().x < tracking_start : ball->getPosition().x > tracking_start)) {
        auto bpos = ball->getPosition();

        if(bpos.y > ppos.y) {
            y =  PADDLE_SPEED;
        } else if(bpos.y < ppos.y) {
            y = -PADDLE_SPEED;
        }


        if(bpos.z > ppos.z) {
            z =  PADDLE_SPEED;
        } else if(bpos.z < ppos.z) {
            z = -PADDLE_SPEED;
        }
    }

    _paddle->setSpeed(Ogre::Vector3(0, y, z));
}
