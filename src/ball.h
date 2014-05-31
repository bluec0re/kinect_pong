#ifndef BALL_H_G4QGINP0
#define BALL_H_G4QGINP0

#include "object.h"

class Ball : public Object
{
public:
    Ball(int color);
    virtual ~Ball ();

    virtual void create(Ogre::SceneManager* sceneMgr);
    virtual void setPosition(const Ogre::Vector3& pos);

private:
};

#endif /* end of include guard: BALL_H_G4QGINP0 */

