#include "global.h"
#include <ExampleApplication.h>

#include <vector>

#define OIS_DYNAMIC_LIB

#include "gamestatemanager.h"
#include "menustate.h"
#include "pong2dstate.h"
#include "pong3dstate.h"

#ifdef HAVE_OPENNI2
#include "kinect.h"
#endif


class Application : public ExampleApplication
{
protected:
    void createScene() {
        mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));
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
        device.guiRenderer = &CEGUI::OgreRenderer::bootstrapSystem();

        CEGUI::ImageManager::setImagesetDefaultResourceGroup("ImageSets");
        CEGUI::Font::setDefaultResourceGroup("Fonts");
        CEGUI::Scheme::setDefaultResourceGroup("Schemes");
        CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
        CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
        CEGUI::AnimationManager::setDefaultResourceGroup("GUIAnimations");


        //

        LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
        OIS::ParamList pl;
        size_t windowHnd = 0;
        std::ostringstream windowHndStr;

        mWindow->getCustomAttribute("WINDOW", &windowHnd);
        windowHndStr << windowHnd;
        pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
#ifndef NDEBUG
    #if defined OIS_WIN32_PLATFORM
        pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
        pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
        pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
        pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
    #elif defined OIS_LINUX_PLATFORM
        pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
        pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
        pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
        pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
    #endif
#endif

        device.InputMgr = OIS::InputManager::createInputSystem( pl );

        //Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
        device.keyboard = static_cast<OIS::Keyboard*>(device.InputMgr->createInputObject( OIS::OISKeyboard, true ));
        device.mouse = static_cast<OIS::Mouse*>(device.InputMgr->createInputObject( OIS::OISMouse, true ));

        GameStateManager gameMgr(&device);

        GameState* menu = MenuState::Create(&gameMgr, "Main Menu");

        GameState* pong2d = Pong2DState::Create(&gameMgr, "Pong 2D");

        GameState* pong3d = Pong3DState::Create(&gameMgr, "Pong 3D");


        gameMgr.start(menu);


        device.InputMgr->destroyInputObject( device.mouse );
        device.InputMgr->destroyInputObject( device.keyboard );

        OIS::InputManager::destroyInputSystem(device.InputMgr);
    }

};


int main(int argc, char *argv[])
{
    Application app;
    app.go();

#ifdef HAVE_OPENNI2
    Kinect::getInstance()->close();
#endif
    return 0;
}
