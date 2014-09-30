#include "kinectplayer.h"
#include "kinect.h"
#include "paddle.h"
#include "playstate.h"
#include <valarray>
#include <cmath>

static Ogre::Real ExponentialMovingAverage( const std::list<Ogre::Real>& data, Ogre::Real baseValue )
{
    size_t size = data.size();
    std::valarray<Ogre::Real> va(size);
    Ogre::Real numerator = 0;
    Ogre::Real denominator = 0;
    size_t i = 0;
    for(std::list<Ogre::Real>::const_iterator it = data.begin();
            it != data.end(); it++, i++) {
        va[i] = *it;
    }

    Ogre::Real average = va.sum();
    average /= size;

    for ( i = 0; i < size; ++i )
    {
        numerator += va[i] * std::pow( baseValue, size - i - 1 );
        denominator += std::pow( baseValue, size - i - 1 );
    }

    numerator += average * std::pow( baseValue, size);
    denominator += std::pow( baseValue, size);

    return numerator / denominator;
}


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
            tmp = static_cast<float>(posRight.x - tl.x)/static_cast<float>(tl.x - br.x);
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

    _samplesX.push_back(current.x);
    _samplesY.push_back(current.y);
    _samplesZ.push_back(current.z);
    while(_samplesX.size() >= _maxSamples) {
        _samplesX.pop_front();
        _samplesY.pop_front();
        _samplesZ.pop_front();
    }

    current.x = ExponentialMovingAverage(_samplesX, 0.9);
    current.y = ExponentialMovingAverage(_samplesY, 0.9);
    current.z = ExponentialMovingAverage(_samplesZ, 0.9);
    ss << " New: " << current;
    Ogre::LogManager::getSingleton().logMessage(ss.str(), Ogre::LML_TRIVIAL);

    return current;
}

bool KinectPlayer::isReady() const {
    return _userid != -1 && _kinect->getTrackingState(_userid) == nite::SKELETON_TRACKED;
}
