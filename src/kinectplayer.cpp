#include "kinectplayer.h"
#include "kinect.h"

KinectPlayer::KinectPlayer(Paddle* paddle, PlayState* playstate) : Player(paddle, playstate) {
    _kinect = Kinect::getInstance();
}
