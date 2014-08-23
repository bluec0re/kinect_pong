#ifndef PLAYER_H_HZOWJQKT
#define PLAYER_H_HZOWJQKT

#include "paddle.h"
#include "kinect.h"

class Kinect;

class Player
{
public:
    Player(Paddle* paddle) : _paddle(paddle) {
        _kinect = Kinect::getInstance();
    };
    virtual int getLives() const { return _lives; }
    virtual bool isAlive() const { return _lives > 0; }
    virtual void die() { _lives--; }
    virtual void setPaddle(Paddle* paddle) { _paddle = paddle; }

    virtual void update(/*double timeSinceLastFrame*/) = 0;

protected:
    int _lives;
    Paddle* _paddle;
    Kinect* _kinect;
};


#endif /* end of include guard: PLAYER_H_HZOWJQKT */

