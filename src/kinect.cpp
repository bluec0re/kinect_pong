#include "kinect.h"
#include "player.h"

Kinect* Kinect::_kinect = nullptr;

void calculateHistogram(float* pHistogram, int histogramSize, const openni::VideoFrameRef& depthFrame)
{
    const openni::DepthPixel* pDepth = (const openni::DepthPixel*)depthFrame.getData();
    int width = depthFrame.getWidth();
    int height = depthFrame.getHeight();
    // Calculate the accumulative histogram (the yellow display...)
    memset(pHistogram, 0, histogramSize*sizeof(float));
    int restOfRow = depthFrame.getStrideInBytes() / sizeof(openni::DepthPixel) - width;

    unsigned int nNumberOfPoints = 0;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x, ++pDepth)
        {
            if (*pDepth != 0)
            {
                pHistogram[*pDepth]++;
                nNumberOfPoints++;
            }
        }
        pDepth += restOfRow;
    }
    for (int nIndex=1; nIndex<histogramSize; nIndex++)
    {
        pHistogram[nIndex] += pHistogram[nIndex-1];
    }
    if (nNumberOfPoints)
    {
        for (int nIndex=1; nIndex<histogramSize; nIndex++)
        {
            pHistogram[nIndex] = (256 * (1.0f - (pHistogram[nIndex] / nNumberOfPoints)));
        }
    }
}

void Kinect::openniFrame2OgreTexture(Ogre::TexturePtr& texture, const openni::VideoFrameRef& depthFrame, float* histogram) {

    float coordinates[2];
    nite::UserId uid = waitForUser(false, false);
    if(uid != -1 && hasControllingHand()) {
        auto leftHand = getJointPosition(getControllingHand(), uid);
        if(userTracker.convertJointCoordinatesToDepth(leftHand.x, leftHand.y, leftHand.z, &coordinates[0], &coordinates[1]) != nite::STATUS_OK)
            uid = -1;
/*        else
            printf("Convert coordinates %fx%f\n", coordinates[0], coordinates[1]);
 */
    }

    int width = depthFrame.getVideoMode().getResolutionX();
    int height = depthFrame.getVideoMode().getResolutionY();

    Ogre::HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();

    pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD); // for best performance use HBL_DISCARD!
    const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();

    Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);

    const openni::DepthPixel* pDepthRow = (const openni::DepthPixel*)depthFrame.getData();
    int rowSize = depthFrame.getStrideInBytes() / sizeof(openni::DepthPixel);
    for (size_t y = 0; y < height; y++)
    {
        const openni::DepthPixel* pDepth = pDepthRow;
        for(size_t x = 0; x < width; x++, pDepth++)
        {
            if (*pDepth != 0)
            {
                int nHistValue = histogram[*pDepth];
                if(uid != -1 && hasControllingHand()) {
                    float markers[2][2];
                    userTracker.convertJointCoordinatesToDepth(_markerPositions[TOP_LEFT].x, _markerPositions[TOP_LEFT].y, _markerPositions[TOP_LEFT].z,
                            &markers[0][0], &markers[0][1]);
                    userTracker.convertJointCoordinatesToDepth(_markerPositions[BOTTOM_RIGHT].x, _markerPositions[BOTTOM_RIGHT].y, _markerPositions[BOTTOM_RIGHT].z,
                            &markers[1][0], &markers[1][1]);

                    if(y >= coordinates[1] - 30 && y <= coordinates[1] + 30 &&
                       x >= coordinates[0] - 30 && x <= coordinates[0] + 30) {
                        *(pDest++) = 0; // R
                        *(pDest++) = nHistValue; // G
                        *(pDest++) = 0; // B
                    } else if(y <= markers[0][1] && y >= markers[1][1] && (fabs(x - markers[0][0]) < 0.01f || fabs(x - markers[1][0]) < 0.01f)) {
                        *(pDest++) = 0; // R
                        *(pDest++) = 0; // G
                        *(pDest++) = 255; // B
                    } else if(x <= markers[0][0] && x >= markers[1][0] && (fabs(y - markers[0][1]) < 0.01f || fabs(y - markers[1][1]) < 0.01f)) {
                        *(pDest++) = 0; // R
                        *(pDest++) = 0; // G
                        *(pDest++) = 255; // B
                    } else {
                        *(pDest++) = nHistValue; // R
                        *(pDest++) = nHistValue; // G
                        *(pDest++) = nHistValue; // B
                    }
                } else {
                    *(pDest++) = nHistValue; // R
                    *(pDest++) = nHistValue; // G
                    *(pDest++) = nHistValue; // B
                }
                *(pDest++) = 255; // A
            } else {
                *(pDest++) = 0;
                *(pDest++) = 0;
                *(pDest++) = 0;
                *(pDest++) = 255;
            }
        }

        pDepthRow += rowSize;

        pDest += pixelBox.getRowSkip() * Ogre::PixelUtil::getNumElemBytes(pixelBox.format);
    }

    // Unlock the pixel buffer
    pixelBuffer->unlock();
/*
    Ogre::Image img;
    texture->convertToImage(img);
    img.save("/tmp/test.png");
*/
}

Kinect::Kinect() : _connected(false) {
    init();
    _markerPositions[TOP_LEFT] = nite::Point3f(0, -MAP_BBOX_Y, 0);
    _markerPositions[TOP_RIGHT] = nite::Point3f(300, -MAP_BBOX_Y, 0);
    _markerPositions[BOTTOM_LEFT] = nite::Point3f(0, MAP_BBOX_Y, 0);
    _markerPositions[BOTTOM_RIGHT] = nite::Point3f(300, MAP_BBOX_Y, 0);
    _markerPositions[CENTER] = nite::Point3f(150, 0.5, 0);
    _hasControllingHand = false;
    _controllingHand = nite::JOINT_RIGHT_HAND;
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


std::vector<nite::UserId> Kinect::getUsers() const {
    //TODO: ueberarbeiten?
    std::vector<nite::UserId> ids;
    for(auto u : _users) {
        ids.push_back(u.first);
    }

    return ids;
}

void Kinect::update() {
    if(!isConnected())
        return;

    nite::UserTrackerFrameRef userTrackerFrame;
    niteRc = userTracker.readFrame(&userTrackerFrame);
    if(niteRc != nite::STATUS_OK) {
        printf("::Failed to get tracker frame");
        return;
    }
    _lastFrame = userTrackerFrame;

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

    const openni::VideoFrameRef& depthFrame = userTrackerFrame.getDepthFrame();
    if(depthFrame.isValid()) {
        calculateHistogram(_depthHist, MAX_DEPTH, depthFrame);
        if(_texture.isNull()) {
            int width = depthFrame.getVideoMode().getResolutionX();
            int height = depthFrame.getVideoMode().getResolutionY();
            printf("::Creating texture %dx%d\n", width, height);
            printf("::Depth Frame %dx%d crop %dx%d\n", depthFrame.getWidth(),
                                                       depthFrame.getHeight(),
                                                       depthFrame.getCropOriginX(),
                                                       depthFrame.getCropOriginY()
                   );
            _texture = Ogre::TextureManager::getSingleton().createManual(
                "kinectDepthMap", // name
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                Ogre::TEX_TYPE_2D,      // type
                width, height,         // width & height
                0,                // number of mipmaps
                Ogre::PF_BYTE_RGBA,     // pixel format
                Ogre::TU_DEFAULT);      // usage; should be TU_DYNAMIC_WRITE_ONLY_DISCARDABLE for
                                  // textures updated very often (e.g. each frame)
        }

        openniFrame2OgreTexture(_texture, depthFrame, _depthHist);
    }
}

Ogre::Vector3 Kinect::getJointPosition(const nite::JointType& type, nite::UserId id) const {
    if(getTrackingState(id) != nite::SKELETON_TRACKED)
        return Ogre::Vector3::ZERO;

    const nite::UserData* user = _lastFrame.getUserById(id);

    const nite::SkeletonJoint& joint = user->getSkeleton().getJoint(type);

    if(joint.getPositionConfidence() > .5) 
        return Ogre::Vector3(joint.getPosition().x,
                             joint.getPosition().y,
                             joint.getPosition().z);

    else
        return Ogre::Vector3::ZERO;
}

nite::SkeletonState Kinect::getTrackingState(nite::UserId userid) const {
    auto user = _lastFrame.getUserById(userid);

    if(user == NULL)
        return nite::SKELETON_NONE;

    return user->getSkeleton().getState();
}

nite::UserId Kinect::waitForUser(bool blocking, bool autoUpdate /* = true*/) {
    if(blocking) {
        printf("::Waiting for user to appear (blocking)\n");
    }
    do {
        if(autoUpdate)
            update();
        // TODO: ueberarbeiten?
        for(auto u : _users) {
            if(getTrackingState(u.first) == nite::SKELETON_TRACKED)
                return u.first;
        }
    } while(blocking && autoUpdate); // blocking without update doesn't make sense

    return -1;
}


Ogre::TexturePtr& Kinect::getDepthImage() {
    return _texture;
}

const Ogre::TexturePtr& Kinect::getDepthImage() const {
    return _texture;
}


const nite::Point3f& Kinect::getRealWorldMarkerPos(const Marker &marker) const {
    size_t idx = static_cast<size_t>(marker);

    if(idx > CENTER)
        throw std::out_of_range("Invalid marker type specified");

    return _markerPositions[idx];
}

void Kinect::setRealWorldMarkerPos(const Marker &marker, const Ogre::Vector3 &pos) {
    setRealWorldMarkerPos(marker, nite::Point3f(pos.x, pos.y, pos.z));
}

void Kinect::setRealWorldMarkerPos(const Marker &marker, const nite::Point3f &pos) {
    size_t idx = static_cast<size_t>(marker);
    if(idx > CENTER)
        throw std::out_of_range("Invalid marker type specified");

    Ogre::StringStream ss;
    ss << "Set Marker ";
    switch (marker) {
        case TOP_LEFT:     ss << "TL to ("; break;
        case TOP_RIGHT:    ss << "TR to ("; break;
        case BOTTOM_LEFT:  ss << "BL to ("; break;
        case BOTTOM_RIGHT: ss << "BR to ("; break;
    }
    ss << pos.x << ", " << pos.y << ", " << pos.z << ")";
    Ogre::LogManager::getSingleton().logMessage(ss.str());

    _markerPositions[marker] = pos;
}

const nite::JointType& Kinect::getControllingHand() const {
    return _controllingHand;
}

void Kinect::setControllingHand(nite::JointType const & hand) {
    _controllingHand = hand;
    hasControllingHand(true);
}

bool Kinect::hasControllingHand() const {
    return _hasControllingHand;
}

void Kinect::hasControllingHand(bool yes) {
    _hasControllingHand = yes;
}

Ogre::Vector2 Kinect::getRelativePosition(const Ogre::Vector3& pos) {
    Ogre::Vector2 out;
    userTracker.convertJointCoordinatesToDepth(pos.x, pos.y, pos.z, &out.x, &out.y);
    out.x /= _lastFrame.getDepthFrame().getVideoMode().getResolutionX();
    out.y /= _lastFrame.getDepthFrame().getVideoMode().getResolutionY();

    return out;
}