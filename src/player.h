#ifndef PLAYER_H_HZOWJQKT
#define PLAYER_H_HZOWJQKT

#include <boost/smart_ptr.hpp>

#include "global.h"

class Paddle;
class PlayState;

class Player
{
public:
    enum Side {
        SIDE_LEFT,
        SIDE_RIGHT,
        SIDE_UNKNOWN
    };

    Player(const Ogre::String& name, Paddle* paddle, PlayState* state) : _name(name), _paddle(paddle), _state(state), _lives(3) {}
    virtual ~Player() {}

    virtual const Ogre::String& getName() const { return _name; }
    virtual int getLives() const { return _lives; }
    virtual bool isAlive() const { return _lives > 0; }
    virtual void hit();
    virtual void setPaddle(Paddle* paddle) { _paddle = paddle; }

    virtual void update(double timeSinceLastFrame=0) = 0;
    virtual bool is2D() const;
    virtual bool is3D() const;
    virtual Side getSide() const;
    virtual bool isReady() const;
    virtual bool isAI() const;

protected:
    const Ogre::String _name;
    int _lives;
    Paddle* _paddle;
    PlayState* _state;
};


typedef boost::shared_ptr<Player> PlayerPtr;

#endif /* end of include guard: PLAYER_H_HZOWJQKT */

