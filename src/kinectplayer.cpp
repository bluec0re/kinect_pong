#include "kinectplayer.h"
#include "kinect.h"
#include "paddle.h"
#include "playstate.h"
#include <iostream>

KinectPlayer::KinectPlayer(const Ogre::String& name, const PaddlePtr& paddle, PlayState* playstate, int userid)
        : Player(name + Ogre::String(" (Kinect)"), paddle, playstate), _userid(userid), _maxSamples(10) {
    _kinect = Kinect::getInstance();
}


void KinectPlayer::update(double timeSinceLastFrame) {
    if(_userid == -1)
        _userid = _kinect->waitForUser(false); // calls update implicit
    else
        _kinect->update();

    if(_kinect->getTrackingState(_userid) == nite::SKELETON_TRACKED) {
        Ogre::Vector3 posRight = getSmoothedPosition();

        Ogre::Vector3 curPos = _paddle->getPosition();

        const nite::Point3f& tl = _kinect->getRealWorldMarkerPos(Kinect::TOP_LEFT);
        const nite::Point3f& tr = _kinect->getRealWorldMarkerPos(Kinect::TOP_RIGHT);
        const nite::Point3f& br = _kinect->getRealWorldMarkerPos(Kinect::BOTTOM_RIGHT);
        const nite::Point3f& bl = _kinect->getRealWorldMarkerPos(Kinect::BOTTOM_LEFT);

        float tmp;
        tmp = static_cast<float>(posRight.y - br.y)/static_cast<float>(tl.y - br.y);
        //std::cout << "Y: " << posRight.y << " -> " << tmp << " => ";
        tmp = tmp*MAP_BBOX_Y*2-MAP_BBOX_Y;
        //std::cout << tmp<< std::endl;
        curPos.y = static_cast<Ogre::Real>(tmp);

        if(is3D()) { // kinect x is z in game
            tmp = static_cast<float>(posRight.x - tl.x)/static_cast<float>(br.x - tl.x);
          //  std::cout << "Z: " << posRight.x << " -> " << tmp << " => ";
            tmp = tmp*MAP_BBOX_Z*2-MAP_BBOX_Z;
            //std::cout << tmp<< std::endl;
            curPos.z = static_cast<Ogre::Real>(tmp);
        }
        _paddle->setPosition(curPos);
    } else
        _userid = -1;
}

Ogre::Vector3 KinectPlayer::getSmoothedPosition() {
    Ogre::StringStream ss;
    Ogre::Vector3 current = _kinect->getJointPosition(_kinect->getControllingHand(), _userid);
    ss << "Current: " << current << " ";

    while(_samples.size() >= _maxSamples) {
        _samples.pop_front();
    }

    size_t cnt = _samples.size();
    Ogre::Vector3 avg(Ogre::Vector3::ZERO);
    if(cnt != 0) {
        for(const Ogre::Vector3& tmp : _samples) {
            avg += tmp / cnt;
        }
        Ogre::Vector3 diff = current - avg;
        Ogre::Real x = fabs(diff.x / (avg.x / 2.0));
        Ogre::Real y = fabs(diff.y / (avg.y / 2.0));
        Ogre::Real z = fabs(diff.z / (avg.z / 2.0));


        diff.x *= x;
        diff.y *= y;
        diff.z *= z;

        current = avg + diff;
    }
    _samples.push_back(current);
    ss << "Average: " << avg << " New: " << current << " Count: " << cnt;
    Ogre::LogManager::getSingleton().logMessage(ss.str());

    return current;
}

bool KinectPlayer::isReady() const {
    return _userid != -1 && _kinect->getTrackingState(_userid) == nite::SKELETON_TRACKED;
}
