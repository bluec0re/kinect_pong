#ifndef PADDLE_H_G4QGINP0
#define PADDLE_H_G4QGINP0

#include "object.h"

class Paddle : public Object
{
public:
    Paddle(int color, const Ogre::String& player);
    virtual ~Paddle ();

    virtual void create(Ogre::SceneManager* sceneMgr);
    virtual void setPosition(const Ogre::Vector3& pos);

private:
    Ogre::String _player;
};

#endif /* end of include guard: PADDLE_H_G4QGINP0 */

