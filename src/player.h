#ifndef PLAYER_H_HZOWJQKT
#define PLAYER_H_HZOWJQKT

class Paddle;

class Player
{
public:
    virtual int getLives() const { return _lives; }
    virtual bool isAlive() const { return _lives > 0; }
    virtual void die() { _lives--; }
    virtual void setPaddle(Paddle* paddle) { _paddle = paddle; }

    virtual void update(double timeSinceLastFrame) = 0;

protected:
    int _lives;
    Paddle* _paddle;
};


#endif /* end of include guard: PLAYER_H_HZOWJQKT */

