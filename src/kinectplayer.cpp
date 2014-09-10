#include "kinectplayer.h"
#include "kinect.h"
#include "paddle.h"
#include "playstate.h"
#include <iostream>

KinectPlayer::KinectPlayer(const Ogre::String& name, Paddle* paddle, PlayState* playstate, int userid) : Player(name + Ogre::String(" (Kinect)"), paddle, playstate), _userid(userid) {
    _kinect = Kinect::getInstance();
}


void KinectPlayer::update(double timeSinceLastFrame) {
    if(_userid == -1)
        _userid = _kinect->waitForUser(false); // calls update implicit
    else
        _kinect->update();

    if(_kinect->getTrackingState(_userid) == nite::SKELETON_TRACKED) {
        Ogre::Vector3 posRight = _kinect->getJointPosition(nite::JOINT_RIGHT_HAND, _userid);

        Ogre::Vector3 curPos = _paddle->getPosition();

        const nite::Point3f& tl = _kinect->getRealWorldMarkerPos(Kinect::TOP_LEFT);
        const nite::Point3f& br = _kinect->getRealWorldMarkerPos(Kinect::BOTTOM_RIGHT);

        float tmp;
        tmp = static_cast<float>(posRight.y - tl.y)/static_cast<float>(br.y - tl.y);
        curPos.y = static_cast<Ogre::Real>(tmp);
        std::cout << "Y: " << posRight.y << " -> " << tmp << std::endl;

        if(is3D()) {
            tmp = static_cast<float>(posRight.x - tl.x)/static_cast<float>(br.x - tl.x);
            std::cout << "Z: " << posRight.x << " -> " << tmp << std::endl;
            curPos.z = static_cast<Ogre::Real>(tmp);
        }
        _paddle->setPosition(curPos);
    } else
        _userid = -1;
}


bool KinectPlayer::isReady() const {
    return _userid != -1 && _kinect->getTrackingState(_userid) == nite::SKELETON_TRACKED;
}
