#ifndef KINECTPLAYER_H
#define KINECTPLAYER_H

#include <NiTE.h>
#include "player.h"

class Kinect;
class Paddle;
class PlayState;

class KinectPlayer : public Player
{
public:
    KinectPlayer(const Ogre::String& name, Paddle* paddle, PlayState* playstate, int userid=-1);

    virtual void update(double timeSinceLastFrame = 0) override;
    virtual bool isReady() const override;

private:
    Kinect* _kinect;
    nite::UserId _userid;
};

#endif // KINECTPLAYER_H
