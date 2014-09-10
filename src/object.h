#ifndef OBJECT_H
#define OBJECT_H

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#include <Ogre.h>
#pragma clang diagnostic pop

class Object
{
public:
    Object() : _color(0xFFFFFF), _entity(0), _node(0), _speed(Ogre::Vector3::ZERO), _accel(Ogre::Vector3::ZERO), _enableAccel(true) {}
    Object(int color) : _color(color), _entity(0), _node(0), _speed(Ogre::Vector3::ZERO), _accel(Ogre::Vector3::ZERO), _enableAccel(true) {}
    virtual ~Object ();

    virtual void create() = 0;

    virtual void setPosition(const Ogre::Vector3& pos) {
        _node->setPosition(pos);
    }
    virtual const Ogre::Vector3& getPosition() const { return _node->getPosition(); }

    virtual void setSpeed(const Ogre::Vector3& speed) { _speed = speed; }
    virtual const Ogre::Vector3& getSpeed() const { return _speed; }

    virtual void setAccel(const Ogre::Vector3& accel) { _accel = accel; }
    virtual const Ogre::Vector3& getAccel() const { return _accel; }

    virtual void setSceneManager(Ogre::SceneManager* sceneMgr) { _sceneMgr = sceneMgr; }

    virtual void enableAccel(bool enable) { _enableAccel = enable; }
    virtual bool isAccelEnabled() const { return _enableAccel; }

    virtual void update(double timeElapsed);

    virtual void accelerate(Ogre::Vector3 accel);

    // null ptr dereference possible!! (But not seen as important here (currently not possible in this code))
    virtual const Ogre::AxisAlignedBox& getBoundingBox() const { return _entity->getWorldBoundingBox(); }

protected:
    int _color;
    bool _enableAccel;
    Ogre::Entity* _entity;
    Ogre::SceneNode* _node;
    Ogre::MaterialPtr _mat;
    Ogre::Vector3 _speed;
    Ogre::Vector3 _accel;
    Ogre::SceneManager *_sceneMgr;
};


#endif // OBJECT_H
