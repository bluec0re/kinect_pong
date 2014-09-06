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

void openniFrame2OgreTexture(Ogre::TexturePtr& texture, const openni::VideoFrameRef& depthFrame, float* histogram) {
    int width = depthFrame.getVideoMode().getResolutionX();
    int height = depthFrame.getVideoMode().getResolutionY();

    Ogre::HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();

    pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL); // for best performance use HBL_DISCARD!
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
                *pDest++ = nHistValue; // L
            }
        }

        pDepthRow += rowSize;

        pDest += pixelBox.getRowSkip() * Ogre::PixelUtil::getNumElemBytes(pixelBox.format);
    }

    // Unlock the pixel buffer
    pixelBuffer->unlock();
}

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


std::vector<int> Kinect::getUsers() const {
    //TODO: ueberarbeiten?
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
            _texture = Ogre::TextureManager::getSingleton().createManual(
                "kinectDepthMap", // name
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                Ogre::TEX_TYPE_2D,      // type
                width, height,         // width & height
                0,                // number of mipmaps
                Ogre::PF_BYTE_L,     // pixel format
                Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);      // usage; should be TU_DYNAMIC_WRITE_ONLY_DISCARDABLE for
                                  // textures updated very often (e.g. each frame)
        }

        openniFrame2OgreTexture(_texture, depthFrame, _depthHist);
    }
}

Ogre::Vector3 Kinect::getJointPosition(const nite::JointType& type, int id) const {
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

nite::SkeletonState Kinect::getTrackingState(int userid) const {
    auto user = _lastFrame.getUserById(userid);

    if(user == NULL)
        return nite::SKELETON_NONE;

    return user->getSkeleton().getState();
}

int Kinect::waitForUser(bool blocking) {
    do {
        update();
        // TODO: ueberarbeiten?
        for(auto u : _users) {
            if(getTrackingState(u.first) == nite::SKELETON_TRACKED)
                return u.first;
        }
    } while(blocking);

    return -1;
}


Ogre::TexturePtr& Kinect::getDepthImage() {
    return _texture;
}

const Ogre::TexturePtr& Kinect::getDepthImage() const {
    return _texture;
}
