#ifndef BALL_H_G4QGINP0
#define BALL_H_G4QGINP0

#include "object.h"

#include <boost/smart_ptr.hpp>

class Ball : public Object
{
public:
    enum Direction {
        DIR_X,
        DIR_Y,
        DIR_Z
    };

    Ball(int color);
    Ball(int colorLeft, int colorRight);
    virtual ~Ball ();

    virtual void create() override;
    virtual void reverse(const Direction& dir, const Ogre::Real& collision = 0);
    virtual void setPosition(const Ogre::Vector3 &pos) override;

protected:
    virtual int calcColor();

private:
    int _color2;
};

typedef boost::shared_ptr<Ball> BallPtr;

#endif /* end of include guard: BALL_H_G4QGINP0 */

