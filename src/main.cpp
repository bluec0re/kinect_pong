#include "global.h"
#include <ExampleApplication.h>

#include <vector>

#include <OgreOggSound/OgreOggSound.h>

#define OIS_DYNAMIC_LIB

#include "gamestatemanager.h"
#include "menustate.h"
#include "settingstate.h"
#include "pong2dstate.h"
#include "pong3dstate.h"

#ifdef HAVE_OPENNI2
    #include "kinect.h"
    #include "kinectcalibrationstate.h"
#endif

#ifdef OIS_LINUX_PLATFORM
    #include <argp.h>
    #include <dlfcn.h>

std::string getModulePathLinux(void* func) {
    Dl_info info;
    dladdr(func, &info);

    return std::string(info.dli_fname);
}
#elif defined(OIS_WIN32_PLATFORM)
#include <Windows.h>

std::string getModulePathLinux(const char* module) {
    HMODULE module = GetModuleHandle(module);
    char buff[256] = {0};
    GetModuleFileName(module, buf, 255);

    return std::string(buf);
}
#endif

struct arguments g_args;

class Application : public ExampleApplication
{
protected:
    void createScene() {
        mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));
    }

    void createFrameListener() {};

public:
    Application() {
        mSceneMgr = nullptr;
    }

    virtual void go(void)
    {
        if (!setup()) {
            LogManager::getSingleton().logMessage(LogMessageLevel::LML_CRITICAL, "!!! Setup cancelled !!!");
            return;
        }

        device_info device;
        device.ogre = mRoot;
        device.rwindow = mWindow;
        device.sceneMgr = mSceneMgr;

        LogManager::getSingleton().logMessage("*** Initializing CEGUI ***");
        if(g_args.debug) {
            // set debug log level
            new CEGUI::DefaultLogger();
            CEGUI::Logger::getSingleton().setLogFilename("CEGUI.log");
            CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);
            CEGUI::Logger::getSingleton().logEvent("Debug logging enabled");

            LogManager::getSingleton().setLogDetail(LoggingLevel::LL_BOREME);
        }

        device.guiRenderer = &CEGUI::OgreRenderer::bootstrapSystem();

        CEGUI::ImageManager::setImagesetDefaultResourceGroup("ImageSets");
        CEGUI::Font::setDefaultResourceGroup("Fonts");
        CEGUI::Scheme::setDefaultResourceGroup("Schemes");
        CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
        CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
        CEGUI::AnimationManager::setDefaultResourceGroup("GUIAnimations");

        Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/sounds", "FileSystem", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, false);
#ifdef OIS_LINUX_PLATFORM
        mRoot->loadPlugin("libOgreOggSound.so");
#elif defined(OIS_WIN32_PLATFORM)
        mRoot->loadPlugin("OgreOggSound.dll");
#endif
        //Instantiate sound manager
        OgreOggSound::OgreOggSoundManager* soundManager = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
        soundManager->init();
        soundManager->createSound("pong", "pong.ogg", false, false, true);
        soundManager->createSound("plus", "plus.ogg", false, false, true);
        soundManager->createSound("minus", "minus.ogg", false, false, true);
        soundManager->createSound("win", "win.ogg", false, false, true);
        soundManager->createSound("lose", "lose.ogg", false, false, true);
        soundManager->createSound("calib", "calib.ogg", false, false, true);
        soundManager->createSound("calib_fin", "calib_fin.ogg", false, false, true);
        soundManager->createSound("calib_fail", "fail.ogg", false, false, true);
        soundManager->createSound("game_loop", "game_loop.ogg", false, true, true);
        soundManager->createSound("menu_loop", "menu_loop.ogg", false, true, true);
        //-------------------------
        LogManager::getSingleton().logMessage("*** Initializing OIS ***");
        OIS::ParamList pl;
        size_t windowHnd = 0;
        std::ostringstream windowHndStr;

        mWindow->getCustomAttribute("WINDOW", &windowHnd);
        windowHndStr << windowHnd;
        pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
#if defined OIS_WIN32_PLATFORM
    #if !defined(NDEBUG)
        pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
        pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
        pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
        pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
    #endif
#elif defined OIS_LINUX_PLATFORM
        if(g_args.nograb) {
            pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
            pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
            pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
            pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
        }
#endif

        device.InputMgr = OIS::InputManager::createInputSystem( pl );

        //Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
        device.keyboard = static_cast<OIS::Keyboard*>(device.InputMgr->createInputObject( OIS::OISKeyboard, true ));
        device.mouse = static_cast<OIS::Mouse*>(device.InputMgr->createInputObject( OIS::OISMouse, true ));

        GameStateManager gameMgr(&device);

        GameState* menu = MenuState::Create(&gameMgr, "Main Menu");

        GameState* settings = SettingState::Create(&gameMgr, "Setting Menu");

#ifdef HAVE_OPENNI2
        GameState* calib = KinectCalibrationState::Create(&gameMgr, "Kinect Calibration");
#endif

        GameState* pong2d = Pong2DState::Create(&gameMgr, "Pong 2D");

        GameState* pong3d = Pong3DState::Create(&gameMgr, "Pong 3D");

        // load game settings
        SettingState::loadSettings();

        if(!g_settings.soundOn) {
            soundManager->muteAllSounds();
        }

        if(strcasecmp(g_args.startState, "pong2d") == 0)
            gameMgr.start(pong2d);
        else if(strcasecmp(g_args.startState, "pong3d") == 0)
            gameMgr.start(pong3d);
#ifdef HAVE_OPENNI2
        else if(strcasecmp(g_args.startState, "calib") == 0)
            gameMgr.start(calib);
#endif
        else if(strcasecmp(g_args.startState, "settings") == 0)
            gameMgr.start(settings);
        else
            gameMgr.start(menu);


        device.InputMgr->destroyInputObject( device.mouse );
        device.InputMgr->destroyInputObject( device.keyboard );

        OIS::InputManager::destroyInputSystem(device.InputMgr);

#ifdef HAVE_OPENNI2
        Kinect::getInstance()->close();
#endif
    }

};

#ifdef OIS_LINUX_PLATFORM
    const char *argp_program_version =
       "kinectpong 1.0";
    const char *argp_program_bug_address =
       "https://github.com/bluec0re/kinect_pong/issues";
     /* Program documentation. */
    static const char *short_doc =
       "KinectPong -- a pong game using Openni";

     /* A description of the arguments we accept. */
    static const char *arg_doc = "";

     /* The options we understand. */
    static struct argp_option poptions[] = {
       {"state",  's', "STATE",      0,  "State to start with (menu, pong2d, pong3d, calib, settings)" },
#ifdef HAVE_OPENCV
       {"record",  'r', 0,      0,  "Record the game into video" },
#endif
       {"debug",  'd', 0,      0,  "Enable debug logging" },
       {"nograb",  'n', 0,      0,  "Disable keyboard/mouse grabbing" },
       { 0 }
     };

     /* Parse a single option. */
     static error_t parse_opt (int key, char *arg, struct argp_state *state)
     {
       /* Get the input argument from argp_parse, which we
          know is a pointer to our arguments structure. */
       struct arguments *arguments = reinterpret_cast<struct arguments*>(state->input);

       switch (key)
         {
         case 's':
           arguments->startState = arg;
           break;

         case 'r':
           arguments->record = true;
           break;

         case 'd':
             arguments->debug = true;
             break;

         case 'n':
             arguments->nograb = true;
             break;

         default:
           return ARGP_ERR_UNKNOWN;
         }
       return 0;
     }

     /* Our argp parser. */
     static struct argp argp = { poptions, parse_opt, arg_doc, short_doc };
#endif

int main(int argc, char *argv[])
{
    //printf("%s\n", getModulePathLinux(Ogre::LogManager::getSingleton).c_str());
    g_args.startState = "menu";
    g_args.record = false;
    g_args.debug = false;
    g_args.nograb = false;
#ifdef OIS_LINUX_PLATFORM
    argp_parse (&argp, argc, argv, 0, 0, &g_args);
#endif
    Application app;
    app.go();

    return 0;
}
