#ifndef __KINECT_H__
#define __KINECT_H__

#include "global.h"
#include <NiTE.h>

#include <vector>
#include <map>

class Player;
//#include "NiteSampleUtilities.h"

#define MAX_USERS 2
#define CHOOSING_BOX_POSITION_X 0
#define CHOOSING_BOX_POSITION_Y 100
#define CHOOSING_BOX_POSITION_RANGE 20
#define MAX_DEPTH 10000


class Kinect
{
    public: 
        enum Marker {
            TOP_LEFT = 0,
            TOP_RIGHT,
            BOTTOM_LEFT,
            BOTTOM_RIGHT,
            CENTER
        };

        static Kinect* getInstance() {
            if(_kinect != nullptr)
                return _kinect;
            else
                return (_kinect = new Kinect());
        }
        virtual ~Kinect();

        void update();
        void init();
        void close();

        int waitForUser(bool blocking);
        bool isConnected() const { return _connected; }

        std::vector<int> getUsers() const;

        nite::SkeletonState getTrackingState(int userid) const;
        Ogre::Vector3 getJointPosition(const nite::JointType& type, int id) const;

        Ogre::TexturePtr& getDepthImage();
        const Ogre::TexturePtr& getDepthImage() const;

        const nite::Point3f& getRealWorldMarkerPos(const Marker& marker) const;
    protected:
        Kinect();
    private: 
        static Kinect* _kinect;
        nite::UserTracker userTracker;
        nite::Status niteRc;
        std::map<int, nite::UserData> _users;
        bool _connected;
        nite::UserTrackerFrameRef _lastFrame;
        Ogre::TexturePtr _texture;
        float	_depthHist[MAX_DEPTH];

        nite::Point3f _markerPositions[5];
};
#endif
