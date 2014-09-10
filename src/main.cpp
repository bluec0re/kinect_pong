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

#ifdef OIS_LINUX_PLATFORM
    #include <argp.h>
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


        //

        LogManager::getSingleton().logMessage("*** Initializing OIS ***");
        OIS::ParamList pl;
        size_t windowHnd = 0;
        std::ostringstream windowHndStr;

        mWindow->getCustomAttribute("WINDOW", &windowHnd);
        windowHndStr << windowHnd;
        pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
#if !defined(NDEBUG)
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

        if(strcasecmp(g_args.startState, "pong2d") == 0)
            gameMgr.start(pong2d);
        else if(strcasecmp(g_args.startState, "pong3d") == 0)
            gameMgr.start(pong3d);
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
     static char doc[] =
       "KinectPong -- a pong game using Openni";

     /* A description of the arguments we accept. */
     static char args_doc[] = "";

     /* The options we understand. */
     static struct argp_option options[] = {
       {"state",  's', "STATE",      0,  "State to start with (menu, pong2d, pong3d)" },
#ifdef HAVE_OPENCV
       {"record",  'r', 0,      0,  "Record the game into video" },
#endif
       {"debug",  'd', 0,      0,  "Enable debug logging" },
       { 0 }
     };

     /* Parse a single option. */
     static error_t
     parse_opt (int key, char *arg, struct argp_state *state)
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

         default:
           return ARGP_ERR_UNKNOWN;
         }
       return 0;
     }

     /* Our argp parser. */
     static struct argp argp = { options, parse_opt, args_doc, doc };
#endif

int main(int argc, char *argv[])
{
    g_args.startState = "menu";
    g_args.record = false;
    g_args.debug = false;
#ifdef OIS_LINUX_PLATFORM
    argp_parse (&argp, argc, argv, 0, 0, &g_args);
#endif
    Application app;
    app.go();
    return 0;
}
