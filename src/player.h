#ifndef PLAYER_H_HZOWJQKT
#define PLAYER_H_HZOWJQKT

#include <boost/smart_ptr.hpp>

class Paddle;
class PlayState;

class Player
{
public:
    Player(Paddle* paddle, PlayState* state) : _paddle(paddle), _state(state) {}
    virtual ~Player() {}

    virtual int getLives() const { return _lives; }
    virtual bool isAlive() const { return _lives > 0; }
    virtual void hit() { _lives--; }
    virtual void setPaddle(Paddle* paddle) { _paddle = paddle; }

    virtual void update(double timeSinceLastFrame=0) = 0;
    virtual bool is2D() const;
    virtual bool is3D() const;

protected:
    int _lives;
    Paddle* _paddle;
    PlayState* _state;
};


typedef boost::shared_ptr<Player> PlayerPtr;

#endif /* end of include guard: PLAYER_H_HZOWJQKT */

