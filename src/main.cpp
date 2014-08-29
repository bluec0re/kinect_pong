#include <NiTE.h>
#include "global.h"
#include <ExampleApplication.h>

#include <vector>

#define OIS_DYNAMIC_LIB

#include "gamestatemanager.h"
#include "pong2dstate.h"
#include "pong3dstate.h"


class Application : public ExampleApplication
{
protected:
    void createScene() {
    }

    void createFrameListener() {};

public:
    virtual void go(void)
    {
        if (!setup())
            return;

        device_info device;
        device.ogre = mRoot;
        device.rwindow = mWindow;
        device.sceneMgr = mSceneMgr;

        LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
        OIS::ParamList pl;
        size_t windowHnd = 0;
        std::ostringstream windowHndStr;

        mWindow->getCustomAttribute("WINDOW", &windowHnd);
        windowHndStr << windowHnd;
        pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

        device.InputMgr = OIS::InputManager::createInputSystem( pl );

        //Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
        device.keyboard = static_cast<OIS::Keyboard*>(device.InputMgr->createInputObject( OIS::OISKeyboard, true ));
        device.mouse = static_cast<OIS::Mouse*>(device.InputMgr->createInputObject( OIS::OISMouse, true ));

        GameStateManager gameMgr(&device);

        GameState* pong2d = Pong2DState::Create(&gameMgr, "Pong 2D");

        GameState* pong3d = Pong3DState::Create(&gameMgr, "Pong 3D");

        gameMgr.start(pong2d);


        device.InputMgr->destroyInputObject( device.mouse );
        device.InputMgr->destroyInputObject( device.keyboard );

        OIS::InputManager::destroyInputSystem(device.InputMgr);
    }

};


void openni2() {
    nite::UserTracker ut;
    openni::Device device;

    openni::Status rc = openni::OpenNI::initialize();
    device.open(openni::ANY_DEVICE);

    nite::NiTE::initialize();

    ut.create(&device);

    nite::UserTrackerFrameRef utf;
    ut.readFrame(&utf);
}

int main(int argc, char *argv[])
{
    Application app;
    app.go();
    return 0;
}
