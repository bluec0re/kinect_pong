#ifndef OBJECT_H
#define OBJECT_H

#include <Ogre.h>

class Object
{
public:
    Object() : _color(0xFFFFFF), _entity(0), _node(0), _speed(Ogre::Vector3::ZERO) {}
    Object(int color) : _color(color), _entity(0), _node(0), _speed(Ogre::Vector3::ZERO) {}
    virtual ~Object () {}

    virtual void create(Ogre::SceneManager* sceneMgr) = 0;

    virtual void setPosition(const Ogre::Vector3& pos) {
        _node->setPosition(pos);
    }
    virtual const Ogre::Vector3& getPosition() const { return _node->getPosition(); }

    virtual void setSpeed(const Ogre::Vector3& speed) { _speed = speed; }
    virtual const Ogre::Vector3& getSpeed() const { return _speed; }
    virtual void update(double timeElapsed) {
        Ogre::Vector3 pos = getPosition();
        pos += _speed * timeElapsed;
        setPosition(pos);
    }

protected:
    int _color;
    Ogre::Entity* _entity;
    Ogre::SceneNode* _node;
    Ogre::MaterialPtr _mat;
    Ogre::Vector3 _speed;
};


#endif // OBJECT_H
