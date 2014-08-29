#ifndef OBJECT_H
#define OBJECT_H

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#include <Ogre.h>
#pragma clang diagnostic pop

class Object
{
public:
    Object() : _color(0xFFFFFF), _entity(0), _node(0), _speed(Ogre::Vector3::ZERO), _accel(Ogre::Vector3::ZERO) {}
    Object(int color) : _color(color), _entity(0), _node(0), _speed(Ogre::Vector3::ZERO), _accel(Ogre::Vector3::ZERO) {}
    virtual ~Object () {}

    virtual void create(Ogre::SceneManager* sceneMgr) = 0;

    virtual void setPosition(const Ogre::Vector3& pos) {
        _node->setPosition(pos);
    }
    virtual const Ogre::Vector3& getPosition() const { return _node->getPosition(); }

    virtual void setSpeed(const Ogre::Vector3& speed) { _speed = speed; }
    virtual const Ogre::Vector3& getSpeed() const { return _speed; }

    virtual void setAccel(const Ogre::Vector3& accel) { _accel = accel; }
    virtual const Ogre::Vector3& getAccel() const { return _accel; }
    virtual void update(double timeElapsed) {
        Ogre::Vector3 pos = getPosition();
        pos += _speed * timeElapsed;
        setPosition(pos);

        // adjust acceleration to speed direction
        Ogre::Vector3 accel = _accel * timeElapsed;
        if(_speed.x < 0)
            accel.x *= -1;
        if(_speed.y < 0)
            accel.y *= -1;
        if(_speed.z < 0)
            accel.z *= -1;

        _speed += accel;
    }

    // null ptr dereference possible!! (But not seen as important here (currently not possible in this code))
    virtual const Ogre::AxisAlignedBox& getBoundingBox() const { return _entity->getWorldBoundingBox(); }

protected:
    int _color;
    Ogre::Entity* _entity;
    Ogre::SceneNode* _node;
    Ogre::MaterialPtr _mat;
    Ogre::Vector3 _speed;
    Ogre::Vector3 _accel;
};


#endif // OBJECT_H
