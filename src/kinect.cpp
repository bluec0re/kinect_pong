#include "kinect.h"
#include "player.h"

Kinect* Kinect::_kinect = nullptr;

Kinect::Kinect() : _connected(false) {
    init();
}

Kinect::~Kinect() {
    close();
}

void Kinect::init() {
    if(_connected)
        return;

    auto version = nite::NiTE::getVersion();
    printf("Using NiTE %d.%d.%db%d\n", version.major,
                                       version.minor,
                                       version.maintenance,
                                       version.build);
    niteRc = nite::NiTE::initialize();
    if(niteRc != nite::STATUS_OK) {
        printf("::Failed to init NiTE\n");
        return;
    }

    niteRc = userTracker.create();
    if(niteRc != nite::STATUS_OK) {
        printf("::Failed to create User Tracker.\n");
    }
    else {
        printf("::User Tracker initialized.\n\n::Start moving around and get detected!\n");
        _connected = true;
    }
}

void Kinect::close() {
    if(_connected) {
        nite::NiTE::shutdown();
        _connected = false;
        printf("::NiTE closed\n");
    }
}

nite::SkeletonJoint Kinect::getLeftHand(nite::UserTrackerFrameRef utf) {
    const nite::Array<nite::UserData>& users = utf.getUsers();
    for(int i = 0; i < users.getSize(); i++) {
        //TBD
    }
}


std::vector<int> Kinect::getUsers() const {
    std::vector<int> ids;
    for(auto u : _users) {
        ids.push_back(u.first);
    }

    return ids;
}

void Kinect::update() {
    nite::UserTrackerFrameRef userTrackerFrame;
    niteRc = userTracker.readFrame(&userTrackerFrame);
    if(niteRc != nite::STATUS_OK) {
        printf("::Failed to get tracker frame");
        return;
    }

    const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();
    for (int i = 0; i < users.getSize(); ++i) {
        const nite::UserData& user = users[i];
        if (user.isNew()) {
            printf("::New User: %d\n", user.getId());
            userTracker.startSkeletonTracking(user.getId());
            _users[user.getId()] = user;
        } else if(user.isLost()) {
            _users.erase(user.getId());
        } else {
            _users[user.getId()] = user;
        }
    }
}

Ogre::Vector3 Kinect::getJointPosition(const nite::JointType& type, int id) const {
    if(getTrackingState(id) != nite::SKELETON_TRACKED)
        return Ogre::Vector3::ZERO;

    const nite::UserData& user = _users.at(id);

    const nite::SkeletonJoint& joint = user.getSkeleton().getJoint(type);

    if(joint.getPositionConfidence() > .5) 
        return Ogre::Vector3(joint.getPosition().x,
                             joint.getPosition().y,
                             joint.getPosition().z);

    else
        return Ogre::Vector3::ZERO;
}

nite::SkeletonState Kinect::getTrackingState(int userid) const {
    auto it = _users.find(userid);

    if(it == _users.end())
        return nite::SKELETON_NONE;

    return it->second.getSkeleton().getState();
}

int Kinect::waitForUser(bool blocking) {
    do {
        update();
        for(auto u : _users) {
            if(getTrackingState(u.first) == nite::SKELETON_TRACKED)
                return u.first;
        }
    } while(blocking);

    return -1;
}
