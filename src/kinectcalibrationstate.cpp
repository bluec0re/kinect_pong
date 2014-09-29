#include <OgreOggSound/OgreOggSound.h>
#include "kinectcalibrationstate.h"
#include "global.h"
#include "kinect.h"

using namespace CEGUI;

void KinectCalibrationState::enter() {
    resume();
}

void KinectCalibrationState::exit() {
    pause();
}

void KinectCalibrationState::resume() {
    _tracked = false;
    _calibrated = false;
    setupWindows();
}

bool KinectCalibrationState::pause() {
    GUIContext& guiContext = System::getSingleton().getDefaultGUIContext();
    guiContext.getMouseCursor().setVisible(false);
    clearAll();
    return true;
}

void KinectCalibrationState::setupWindows() {
    WindowManager& wmgr = WindowManager::getSingleton();
    GUIContext& guiContext = System::getSingleton().getDefaultGUIContext();

    SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
    ImageManager::getSingleton().loadImageset("AlfiskoSkin.imageset", "ImageSets");

    guiContext.getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
    guiContext.getMouseCursor().setVisible(true);

    Window* root = wmgr.createWindow("DefaultWindow");
    guiContext.setRootWindow(root);

    Window* text = wmgr.createWindow("TaharezLook/Label", "KinectStatusLabel");
    text->setText("Kinect not connected!");
    text->setSize(USize(UDim(1.0, 0), UDim(1.0, 0)));
    text->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    text->setProperty("HorizontalAlignment", "Centre");
    root->addChild(text);


    Window* btn = wmgr.createWindow("TaharezLook/Button", "KeyboardBtn");
    btn->setText("Use Keyboard");
    btn->setSize(USize(UDim(0.23, 0), UDim(0.03, 0)));
    btn->setPosition(UVector2(UDim(0, 0), UDim(0.6, 0)));
    btn->setProperty("HorizontalAlignment", "Centre");
    btn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KinectCalibrationState::handleKeyboardBtnClick, this));
    root->addChild(btn);

    if(Kinect::getInstance()->isConnected()) {
        setupKinectWindow();
    }
}

void KinectCalibrationState::setupKinectWindow() {
    GUIContext& guiContext = System::getSingleton().getDefaultGUIContext();
    guiContext.getMouseCursor().hide();
    WindowManager& wmgr = WindowManager::getSingleton();
    Window* root = guiContext.getRootWindow();

    root->destroyChild("KinectStatusLabel");
    root->destroyChild("KeyboardBtn");

    Logger::getSingleton().logEvent("Creating Kinect Window");
    Kinect::getInstance()->update();
    // preview

    Texture& cetex = mDevice->guiRenderer->createTexture("depthMap", Kinect::getInstance()->getDepthImage());

    const Rectf rect(Vector2f(0.0f, 0.0f), cetex.getOriginalDataSize());
    BasicImage* image = dynamic_cast<BasicImage*>(&ImageManager::getSingleton().create("BasicImage", "KinectImage"));
    image->setTexture(&cetex);
    image->setArea(rect);
    image->setAutoScaled(ASM_Both);

    Window* si = wmgr.createWindow("TaharezLook/StaticImage", "KinectWindow");
    si->setSize(USize(UDim(1.0, 0), UDim(1.0, 0)));

    si->setProperty("Image", "KinectImage");
    root->addChild(si);
    Logger::getSingleton().logEvent("Kinect Window created");


    ImageManager::getSingleton().addFromImageFile("CalibrationManual", "manual_x64_base.png", "Images");
    ImageManager::getSingleton().addFromImageFile("CalibrationManualTracked", "manual_x64_tracked.png", "Images");
    ImageManager::getSingleton().addFromImageFile("CalibrationManualCalibrated", "manual_x64_calibrated.png", "Images");

    _manual = wmgr.createWindow("TaharezLook/StaticImage", "CalibrationWindow");
    _manual->setSize(USize(UDim(0, 230), UDim(0, 64)));
    _manual->setPosition(UVector2(UDim(0.5, -115), UDim(0, 0)));

    _manual->setProperty("Image", "CalibrationManual");
    _manual->setProperty("FrameEnabled", "false");
    si->addChild(_manual);

    _progress = dynamic_cast<ProgressBar*>(wmgr.createWindow("TaharezLook/ProgressBar", "CalibrationProgress"));
    _progress->setSize(USize(UDim(0, 200), UDim(0, 20)));
    _progress->setPosition(UVector2(UDim(0.5, -100), UDim(0, 64)));
    si->addChild(_progress);


    _target = wmgr.createWindow("TaharezLook/StaticImage", "Target");
    _target->setSize(USize(UDim(0, 20), UDim(0, 20)));
    _target->setPosition(UVector2(UDim(0.5, -10), UDim(0.3, -10)));
    _target->setProperty("Image", "AlfiskoSkin/CloseButtonPushed");
    _target->setProperty("FrameEnabled", "false");
    _target->setProperty("BackgroundEnabled", "false");
    _target->setVisible(false);
    si->addChild(_target);
}

bool KinectCalibrationState::handleKeyboardBtnClick(const CEGUI::EventArgs &args) {
    changeGameState(findByName("Pong 2D"));
    return true;
}

bool KinectCalibrationState::frameRenderingQueued(const Ogre::FrameEvent& evt) {
    Kinect* kinect = Kinect::getInstance();
    if(kinect->isConnected()) {
        nite::UserId uid = kinect->waitForUser(false, true);
        OgreOggSound::OgreOggSoundManager* soundManager = OgreOggSound::OgreOggSoundManager::getSingletonPtr();

        if(!_tracked && uid != -1) {
            _tracked = true;
            _calibrated = false;
            _progress->setProgress(0.3333f);
            soundManager->getSound("calib")->play();
            _manual->setProperty("Image", "CalibrationManualTracked");
            _tPoseTimeout = 3.0f;
            _positions.clear();
            kinect->hasControllingHand(false);
        } else if(_tracked && !_calibrated) {
            if (uid == -1) {
                _tracked = false;
                _progress->setProgress(0.f);
                soundManager->getSound("calib_fail")->play();
                _manual->setProperty("Image", "CalibrationManual");
            } else {
                Ogre::Vector3 head = kinect->getJointPosition(nite::JOINT_HEAD, uid);
                Ogre::Vector3 left = kinect->getJointPosition(nite::JOINT_LEFT_HAND, uid);
                Ogre::Vector3 right = kinect->getJointPosition(nite::JOINT_RIGHT_HAND, uid);
                Ogre::Vector3 torso = kinect->getJointPosition(nite::JOINT_TORSO, uid);

                if(head.y - 20 > left.y && head.y - 20 > right.y &&
                        torso.y + 20 < left.y && torso.y + 20 < right.y &&
                        left.x < head.x && right.x > head.x) {
                    float progress = 0.3333f + 0.3333f * (1.f - _tPoseTimeout / 3.0f);
                    soundManager->getSound("calib")->play();
                    printf("progress: %f\n", progress);
                    _progress->setProgress(progress);
                    _tPoseTimeout -= evt.timeSinceLastFrame;

                    _positions.push_back(std::pair<Ogre::Vector3, Ogre::Vector3>(left, right));

                    if(_tPoseTimeout < 0) {
                        updateMarkers();
                        _manual->setProperty("Image", "CalibrationManualCalibrated");
                        _calibrated = true;
                        _target->setVisible(true);
                        _tPoseTimeout = -1.f;
                    }
                }
            }
        } else if(_tracked && _calibrated && uid != -1) {
            if(!kinect->hasControllingHand()) {
                nite::JointType types[] = {nite::JOINT_LEFT_HAND, nite::JOINT_RIGHT_HAND};
                for(nite::JointType& type : types) {
                    Ogre::Vector2 pos = kinect->getRelativePosition(kinect->getJointPosition(type, uid));
                    if(pos.x > 0.45 && pos.x < 0.55 && pos.y > 0.25 && pos.y < 0.35) {
                        _progress->setProgress(1.f);
                        soundManager->getSound("calib_fin")->play();
                        kinect->setControllingHand(type);
                        Ogre::StringStream ss;
                        ss << "Using hand " << type;
                        Ogre::LogManager::getSingleton().logMessage(ss.str());
                        updateMarkers();
                        _tPoseTimeout = 3.f;
                        break;
                    }
                }
            } else {
                _tPoseTimeout -= evt.timeSinceLastFrame;
                if(_tPoseTimeout < 0) {
                    changeGameState(findByName("Pong 2D"));
                }
            }
        }
    }

    return true;
}

void KinectCalibrationState::updateMarkers() {
    Kinect* kinect = Kinect::getInstance();
    Ogre::Vector3 left, right, middle;
    for(auto entry : _positions) {
        left += entry.first;
        right += entry.second;
    }
    left /= _positions.size();
    right /= _positions.size();
    middle = (left + right) / 2;
    {
        Ogre::StringStream ss;
        ss << "Detected Average " << left << " " << middle << " " << right;
        Ogre::LogManager::getSingleton().logMessage(ss.str());
    }
    nite::Point3f tl, tr, bl, br;

    if(!kinect->hasControllingHand()) {
        auto tmp = (left + middle) / 2;
        tl.x = tmp.x;
        tl.y = tmp.y * 1.3f;
        tl.z = tmp.z;

        tmp = (right + middle) / 2;
        tr.x = tmp.x;
        tr.y = tmp.y * 1.3f;
        tr.z = tmp.z;

        tmp = (left + middle) / 2;
        bl.x = tmp.x;
        bl.y = tmp.y * 0.7f;
        bl.z = tmp.z;

        tmp = (right + middle) / 2;
        br.x = tmp.x;
        br.y = tmp.y * 0.7f;
        br.z = tmp.z;
    } else if(kinect->getControllingHand() == nite::JOINT_RIGHT_HAND) {
        auto tmp = (left + middle) / 2;
        tl.x = middle.x;
        tl.y = tmp.y * 1.3f;
        tl.z = tmp.z;

        tmp = (right + middle) / 2;
        tr.x = right.x;
        tr.y = tmp.y * 1.3f;
        tr.z = tmp.z;

        tmp = (left + middle) / 2;
        bl.x = middle.x;
        bl.y = tmp.y * 0.7f;
        bl.z = tmp.z;

        tmp = (right + middle) / 2;
        br.x = right.x;
        br.y = tmp.y * 0.7f;
        br.z = tmp.z;
    } else if(kinect->getControllingHand() == nite::JOINT_LEFT_HAND) {
        auto tmp = (left + middle) / 2;
        tl.x = left.x;
        tl.y = tmp.y * 1.3f;
        tl.z = tmp.z;

        tmp = (right + middle) / 2;
        tr.x = middle.x;
        tr.y = tmp.y * 1.3f;
        tr.z = tmp.z;

        tmp = (left + middle) / 2;
        bl.x = left.x;
        bl.y = tmp.y * 0.7f;
        bl.z = tmp.z;

        tmp = (right + middle) / 2;
        br.x = middle.x;
        br.y = tmp.y * 0.7f;
        br.z = tmp.z;
    }

    {
        Ogre::StringStream ss;
        ss << "TL " << tl.x << "," << tl.y << ", " << tl.z << "\n";
        ss << "TR " << tr.x << "," << tr.y << ", " << tr.z << "\n";
        ss << "BR " << br.x << "," << br.y << ", " << br.z << "\n";
        ss << "BL " << bl.x << "," << bl.y << ", " << bl.z;
        Ogre::LogManager::getSingleton().logMessage(ss.str());
    }

    kinect->setRealWorldMarkerPos(Kinect::TOP_LEFT, tl);
    kinect->setRealWorldMarkerPos(Kinect::TOP_RIGHT, tr);
    kinect->setRealWorldMarkerPos(Kinect::BOTTOM_LEFT, bl);
    kinect->setRealWorldMarkerPos(Kinect::BOTTOM_RIGHT, br);
}
