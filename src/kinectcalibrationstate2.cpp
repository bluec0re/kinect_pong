#include <OgreOggSound/OgreOggSound.h>
#include "kinectcalibrationstate2.h"
#include "global.h"
#include "kinect.h"

using namespace CEGUI;

bool debug = false;

void KinectCalibrationState2::enter() {
    resume();
}

void KinectCalibrationState2::exit() {
    pause();
}

void KinectCalibrationState2::resume() {
    _tracked = false;
    _calibrated = false;
    setupWindows();
}

bool KinectCalibrationState2::pause() {
    GUIContext& guiContext = System::getSingleton().getDefaultGUIContext();
    guiContext.getMouseCursor().setVisible(false);
    clearAll();
    _setuped = false;
    return true;
}


bool KinectCalibrationState2::mouseMoved( const OIS::MouseEvent &arg ) {
    if(debug || !Kinect::getInstance()->isConnected())
        return GuiState::mouseMoved(arg);

    return true;
}

void KinectCalibrationState2::setupWindows() {
    WindowManager& wmgr = WindowManager::getSingleton();
    GUIContext& guiContext = System::getSingleton().getDefaultGUIContext();

    SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
    ImageManager::getSingleton().loadImageset("Calibration.imageset", "ImageSets");


    if(Kinect::getInstance()->isConnected() || debug) {
        guiContext.getMouseCursor().hide();
        Window* root = wmgr.loadLayoutFromFile("CalibrationStateKinect2.layout");
        guiContext.setRootWindow(root);
        setupKinectWindow();
    } else {
        guiContext.getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
        guiContext.getMouseCursor().setVisible(true);

        Window* root = wmgr.loadLayoutFromFile("CalibrationStateKeyboard.layout");
        guiContext.setRootWindow(root);
        Window* btn = root->getChild("KeyboardBtn");
        btn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KinectCalibrationState2::handleKeyboardBtnClick, this));
    }
    _setuped = true;
}

void KinectCalibrationState2::setupKinectWindow() {
    GUIContext& guiContext = System::getSingleton().getDefaultGUIContext();

    WindowManager& wmgr = WindowManager::getSingleton();
    Window* root = guiContext.getRootWindow();

    Logger::getSingleton().logEvent("Creating Kinect Window");
    Kinect::getInstance()->update();
    // preview

    if(!debug) {
        Texture &cetex = mDevice->guiRenderer->createTexture("depthMap", Kinect::getInstance()->getDepthImage());

        const Rectf rect(Vector2f(0.0f, 0.0f), cetex.getOriginalDataSize());
        BasicImage *image = dynamic_cast<BasicImage *>(&ImageManager::getSingleton().create("BasicImage", "KinectImage"));
        image->setTexture(&cetex);
        image->setArea(rect);
        image->setAutoScaled(ASM_Both);
    }

    Window* si = root->getChild("KinectWindow");
    if(!debug)
        si->setProperty("Image", "KinectImage");
    Logger::getSingleton().logEvent("Kinect Window created");

    _progress = dynamic_cast<ProgressBar*>(si->getChild("CalibrationProgress"));

    _status = si->getChild("infoFrame")->getChild("StatusLbl");

    _target = si->getChild("Target");
    _target->setVisible(false);

    _markerArea = si->getChild("Area");

    _markerArea->setVisible(debug);

    uint32 h = mDevice->rwindow->getHeight();
    uint32 w = mDevice->rwindow->getWidth();

    MouseCursor& mc = guiContext.getMouseCursor();
    mc.setImage("Calibration/MouseTarget");
    /*mc.setPosition(Vector2f(
            w * 0.5, h * 0.5
    ));*/
    mc.show();

    _lastPos.x = nan("");
}


bool KinectCalibrationState2::handleKeyboardBtnClick(const CEGUI::EventArgs &args) {
    if(g_settings.skip2D)
        changeGameState(findByName("Pong 3D"));
    else
        changeGameState(findByName("Pong 2D"));
    return true;
}

bool KinectCalibrationState2::frameRenderingQueued(const Ogre::FrameEvent& evt) {
    Kinect* kinect = Kinect::getInstance();
    OgreOggSound::OgreOggSoundManager &soundManager = OgreOggSound::OgreOggSoundManager::getSingleton();

    if(_setuped && kinect->isConnected() || debug) {
        nite::UserId uid = kinect->waitForUser(false, true);
        if(uid == -1 && !debug) {
            if(_tracked) {
                _tracked = false;
                _status->setText("Get detected");
                soundManager.getSound("calib_fail")->play();
                _topLeft = _topRight = _bottomLeft = _bottomRight = false;
            }
        } else if(!_handSelected && !debug) {
            if(!_tracked) {
                soundManager.getSound("calib")->play();
                _status->setText("Move controller hand to target");
                _tracked = true;
            }
            selectHand();
        } else if(!(_topLeft && _topRight && _bottomLeft && _bottomRight)) {
            markArea();
        } else {
            Ogre::StringStream ss2;
            ss2 << "Game starts in " << _poseTimeout << "s";
            _status->setText(ss2.str());
            if(_poseTimeout <= 0) {
                if(g_settings.skip2D)
                    changeGameState(findByName("Pong 3D"));
                else
                    changeGameState(findByName("Pong 2D"));
            }
        }
    }

    if(_poseTimeout > 0)
        _poseTimeout -= evt.timeSinceLastFrame;

    return true;
}

Ogre::Vector3 KinectCalibrationState2::getCurrentPosition() {
    if(debug) {
        return Ogre::Vector3::ZERO;
    } else {
        Kinect* kinect = Kinect::getInstance();
        nite::UserId uid = kinect->waitForUser(false, false);
        return kinect->getJointPosition(kinect->getControllingHand(), uid);
    }
}

Ogre::Vector2 KinectCalibrationState2::getCurrentRelPosition() {
    if(debug) {
        GUIContext& guiContext = System::getSingleton().getDefaultGUIContext();
        Vector2f mousePos = guiContext.getMouseCursor().getPosition();
        uint32 h = mDevice->rwindow->getHeight();
        uint32 w = mDevice->rwindow->getWidth();
        return Ogre::Vector2(mousePos.d_x/w, mousePos.d_y/h);
    } else {
        Kinect* kinect = Kinect::getInstance();
        return kinect->getRelativePosition(getCurrentPosition());
    }
}

void KinectCalibrationState2::selectHand() {
    OgreOggSound::OgreOggSoundManager &soundManager = OgreOggSound::OgreOggSoundManager::getSingleton();
    GUIContext& guiContext = System::getSingleton().getDefaultGUIContext();

    Kinect *kinect = Kinect::getInstance();
    nite::UserId uid = kinect->waitForUser(false, false);

    nite::JointType types[] = {nite::JOINT_LEFT_HAND, nite::JOINT_RIGHT_HAND};
    for (nite::JointType &type : types) {
        Ogre::Vector2 pos = kinect->getRelativePosition(kinect->getJointPosition(type, uid));
        if (pos.x > 0.45 && pos.x < 0.55 && pos.y > 0.45 && pos.y < 0.55) {
            _progress->setProgress(.2f);
            soundManager.getSound("calib")->play();
            kinect->setControllingHand(type);
            Ogre::StringStream ss;
            ss << "Using hand " << type;
            Ogre::LogManager::getSingleton().logMessage(ss.str());

            _handSelected = true;
            _lastPos.x = nan("");
            _lastPos.y = nan("");
            MouseCursor& mc = guiContext.getMouseCursor();
            mc.setImage("TaharezLook/MouseMoveCursor");
            _markerArea->setVisible(true);
            return;
        }
    }
}


void KinectCalibrationState2::markArea() {
    GUIContext& guiContext = System::getSingleton().getDefaultGUIContext();
    Kinect* kinect = Kinect::getInstance();
    OgreOggSound::OgreOggSoundManager& soundManager = OgreOggSound::OgreOggSoundManager::getSingleton();

    Ogre::Vector2 pos = getCurrentRelPosition();
    Ogre::Vector3 kinectPos = getCurrentPosition();

    uint32 h = mDevice->rwindow->getHeight();
    uint32 w = mDevice->rwindow->getWidth();
    MouseCursor& mc = guiContext.getMouseCursor();
    mc.setPosition(Vector2f(
            w * pos.x, h * pos.y
    ));


    Ogre::StringStream ss;
    ss << "Calibrating " << pos << " ";

    const Ogre::Real maxDist = 0.005;

    if(_lastPos.isNaN()) {
        ss << "STARTED ";
        _lastPos = pos;
        _poseTimeout = 5;
    } else if(_lastPos.squaredDistance(pos) > maxDist) {
        // reset timeout
        _lastPos.x = nan("");
        ss << "FAILED ";
    } else
        _lastPos = pos;

    Ogre::StringStream ss2;
    ss2 << "Saving ";

    if(!_topLeft) {
        ss << "TL ";
        ss2 << "top left";
        if(_poseTimeout <= 0) {
            _topLeft = true;
            // bypass topright
            _topRight = true;
            _lastPos.x = nan("");
            ss << "DONE";
            _progress->setProgress(.4f);
            soundManager.getSound("calib")->play();
            kinect->setRealWorldMarkerPos(Kinect::TOP_LEFT, kinectPos);
        } else {
            URect area = _markerArea->getArea();
            area.top(UDim(pos.y, 0));
            area.left(UDim(pos.x, 0));
            _markerArea->setArea(area);
        }
    } else if(!_topRight) {
        ss << "TR ";
        ss2 << "top right";
        if(_poseTimeout <= 0) {
            _topRight = true;
            _lastPos.x = nan("");
            ss << "DONE";
            _progress->setProgress(.6f);
            soundManager.getSound("calib")->play();
            kinect->setRealWorldMarkerPos(Kinect::TOP_RIGHT, kinectPos);
        }  else {
            URect area = _markerArea->getArea();
            area.top(UDim(pos.y, 0));
            area.right(UDim(pos.x, 0));
            _markerArea->setArea(area);
        }
    } else if(!_bottomRight) {
        ss << "BR ";
        ss2 << "bottom right";
        if(_poseTimeout <= 0) {
            _bottomRight = true;
            // bypass bottom left
            _bottomLeft = true;
            _lastPos.x = nan("");
            ss << "DONE";
            _progress->setProgress(.8f);
            soundManager.getSound("calib")->play();
            kinect->setRealWorldMarkerPos(Kinect::BOTTOM_RIGHT, kinectPos);
            nite::Point3f tl = kinect->getRealWorldMarkerPos(Kinect::TOP_LEFT);
            nite::Point3f tmp = tl;
            tmp.x = kinectPos.x;
            kinect->setRealWorldMarkerPos(Kinect::TOP_RIGHT, tmp);
            tmp = tl;
            tmp.y = kinectPos.y;
            kinect->setRealWorldMarkerPos(Kinect::BOTTOM_LEFT, tmp);
        }  else {
            URect area = _markerArea->getArea();
            area.bottom(UDim(pos.y, 0));
            area.right(UDim(pos.x, 0));
            _markerArea->setArea(area);
        }
    } else if(!_bottomLeft) {
        ss << "BL ";
        ss2 << "bottom left";
        if(_poseTimeout <= 0) {
            _bottomLeft = true;
            _lastPos.x = nan("");
            ss << "DONE";
            _progress->setProgress(1.f);
            soundManager.getSound("calib_fin")->play();
            kinect->setRealWorldMarkerPos(Kinect::BOTTOM_LEFT, kinectPos);
            _poseTimeout = 3;
        }  else {
            URect area = _markerArea->getArea();
            area.bottom(UDim(pos.y, 0));
            area.left(UDim(pos.x, 0));
            _markerArea->setArea(area);
        }
    }

    Ogre::LogManager::getSingleton().logMessage(ss.str(), Ogre::LML_TRIVIAL);

    ss2 << " position in " << _poseTimeout << "s";
    _status->setText(ss2.str());
}
