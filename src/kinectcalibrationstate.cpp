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
    clearAll();
    return true;
}

void KinectCalibrationState::setupWindows() {
    WindowManager& wmgr = WindowManager::getSingleton();
    GUIContext& guiContext = System::getSingleton().getDefaultGUIContext();

    SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
    guiContext.getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");

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
    btn->setSize(USize(UDim(0, 160), UDim(0, 48)));
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


    ImageManager::getSingleton().addFromImageFile("CalibrationManual", "manual_x64.png", "Images");

    Window* manual = wmgr.createWindow("TaharezLook/StaticImage", "CalibrationWindow");
    manual->setSize(USize(UDim(0, 230), UDim(0, 64)));
    manual->setPosition(UVector2(UDim(0.0, 0), UDim(0, 0)));

    manual->setProperty("Image", "CalibrationManual");
    manual->setProperty("FrameColours", "tl:00000000 tr:00000000 bl:00000000 br:00000000");
    si->addChild(manual);

    _progress = dynamic_cast<ProgressBar*>(wmgr.createWindow("TaharezLook/ProgressBar", "CalibrationProgress"));
    _progress->setSize(USize(UDim(0, 200), UDim(0, 20)));
    _progress->setPosition(UVector2(UDim(0.0, 10), UDim(0, 64)));
    si->addChild(_progress);
}

bool KinectCalibrationState::handleKeyboardBtnClick(const CEGUI::EventArgs &args) {
    changeGameState(findByName("Pong 2D"));
    return true;
}

bool KinectCalibrationState::frameRenderingQueued(const Ogre::FrameEvent& evt) {
    Kinect* kinect = Kinect::getInstance();
    if(kinect->isConnected()) {
        nite::UserId uid = kinect->waitForUser(false, true);

        if(!_tracked && uid != -1) {
            _tracked = true;
            _calibrated = false;
            _progress->setProgress(0.3333f);
            _tPoseTimeout = 3.0f;
        } else if(_tracked) {
            if (uid == -1) {
                _tracked = false;
                _progress->setProgress(0.f);
            } else if (!_calibrated) {
                float progress = 0.3333f + 0.3333f * (1.f - _tPoseTimeout / 3.0f);
                printf("progress: %f\n", progress);
                _progress->setProgress(progress);
                _tPoseTimeout -= evt.timeSinceLastFrame;
            }
        }
    }

    return true;
}