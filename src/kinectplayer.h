#ifndef KINECTPLAYER_H
#define KINECTPLAYER_H

#include <NiTE.h>
#include "player.h"

class Kinect;
class PlayState;

class KinectPlayer : public Player
{
public:
    KinectPlayer(const Ogre::String& name, const PaddlePtr& paddle, PlayState* playstate, int userid=-1);

    virtual void update(double timeSinceLastFrame = 0) override;
    virtual bool isReady() const override;

protected:
    Ogre::Vector3 getSmoothedPosition();

private:
    Kinect* _kinect;
    nite::UserId _userid;
    std::list<Ogre::Real> _samplesX;
    std::list<Ogre::Real> _samplesY;
    std::list<Ogre::Real> _samplesZ;
    size_t _maxSamples;
};

#endif // KINECTPLAYER_H
