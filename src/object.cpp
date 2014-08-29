#include "object.h"


void Object::update(double timeElapsed) {
    Ogre::Vector3 pos = getPosition();
    pos += _speed * timeElapsed;
    setPosition(pos);

    if(_enableAccel) {
        Ogre::Vector3 accel = _accel * timeElapsed;
        accelerate(accel);
    }
}

void Object::accelerate(Ogre::Vector3 accel) {
    // adjust acceleration to speed direction
    if(_speed.x < 0)
        accel.x *= -1;
    if(_speed.y < 0)
        accel.y *= -1;
    if(_speed.z < 0)
        accel.z *= -1;

    _speed += accel;
}
