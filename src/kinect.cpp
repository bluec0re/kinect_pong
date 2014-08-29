#include "kinect.h"
#include "player.h"

Kinect* Kinect::_kinect = nullptr;

Kinect::Kinect() {
    niteRc = userTracker.create();
    if(niteRc != nite::STATUS_OK) {
        printf("::Failed to create User Tracker.");
    }
    else {
        printf("::User Tracker initialized.\n\n::Start moving around and get detected!");
    }
}

nite::SkeletonJoint Kinect::getLeftHand(nite::UserTrackerFrameRef utf) {
    const nite::Array<nite::UserData>& users = utf.getUsers();
    for(int i = 0; i < users.getSize(); i++) {
        //TBD
    }
}

void Kinect::changeState() {
    _player->update();
}


