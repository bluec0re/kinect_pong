#include "kinectplayer.h"
#include "kinect.h"

KinectPlayer::KinectPlayer(const Ogre::String& name, Paddle* paddle, PlayState* playstate) : Player(name, paddle, playstate) {
    _kinect = Kinect::getInstance();
}
