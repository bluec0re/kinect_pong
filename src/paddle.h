#ifndef PADDLE_H_G4QGINP0
#define PADDLE_H_G4QGINP0

#include "object.h"

#include <boost/smart_ptr.hpp>

class Paddle : public Object
{
public:
    Paddle(int color, const Ogre::String& player, bool transparent=false);
    virtual ~Paddle ();

    virtual void create(Ogre::SceneManager* sceneMgr);
    virtual void setPosition(const Ogre::Vector3& pos);

private:
    Ogre::String _player;
    bool _transparent;
};

typedef boost::shared_ptr<Paddle> PaddlePtr;

#endif /* end of include guard: PADDLE_H_G4QGINP0 */

