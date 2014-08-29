#ifndef KINECTPLAYER_H
#define KINECTPLAYER_H

#include "player.h"

class Kinect;
class Paddle;
class PlayState;

class KinectPlayer : public Player
{
public:
    KinectPlayer(const Ogre::String& name, Paddle* paddle, PlayState* playstate);

private:
    Kinect* _kinect;
};

#endif // KINECTPLAYER_H
