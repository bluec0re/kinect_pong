#include "kinectplayer.h"
#include "kinect.h"
#include "paddle.h"
#include "playstate.h"

KinectPlayer::KinectPlayer(const Ogre::String& name, Paddle* paddle, PlayState* playstate, int userid) : Player(name, paddle, playstate), _userid(userid) {
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
        // TODO: normalize
        curPos.y = posRight.y;
        if(is3D())
            curPos.z = posRight.z;
        _paddle->setPosition(curPos);
    } else
        _userid = -1;
}
