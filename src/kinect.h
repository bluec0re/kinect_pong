#ifndef __KINECT_H__
#define __KINECT_H__

#include "NiTE.h"

class Player;
//#include "NiteSampleUtilities.h"

#define MAX_USERS 2
#define CHOOSING_BOX_POSITION_X 0
#define CHOOSING_BOX_POSITION_Y 100
#define CHOOSING_BOX_POSITION_RANGE 20

class Kinect
{
    public: 
        static Kinect* getInstance() {
            if(_kinect != nullptr)
                return _kinect;
            else
                return (_kinect = new Kinect());
        }
        ~Kinect() {
            if(_kinect != nullptr)
                delete _kinect;
        };
        void setPlayer(Player* player) {_player = player;};
        void changeState();

        nite::SkeletonJoint getLeftHand(nite::UserTrackerFrameRef utf);
        nite::SkeletonJoint getRightHand(nite::UserTrackerFrameRef utf);
    protected:
        Kinect();
    private: 
        Player* _player;
        static Kinect* _kinect;
        nite::UserTracker userTracker;
        nite::Status niteRc;
};
#endif
