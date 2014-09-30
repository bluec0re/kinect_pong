#ifndef GLOBAL_H_YO8PBGIO
#define GLOBAL_H_YO8PBGIO


#include <vector>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#include <Ogre.h>
#pragma clang diagnostic pop
#include <OIS.h>
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>


class GameStateManager;

typedef struct
{
    Ogre::Root *ogre;
    Ogre::RenderWindow *rwindow;
    Ogre::SceneManager *sceneMgr;
    OIS::InputManager *InputMgr;
    OIS::Keyboard *keyboard;
    OIS::Mouse *mouse;
    CEGUI::OgreRenderer *guiRenderer;
} device_info;

#define MAP_SIZE 200
// TODO: create map from following vars
#define MAP_X 400
#define MAP_Y 200
#define MAP_Z 200
#define MAP_BBOX 190
// TODO: create map from following vars
#define MAP_BBOX_X 390
#define MAP_BBOX_Y 190
#define MAP_BBOX_Z 190



struct arguments
{
    const char *startState;
    bool record;
    bool debug;
    bool nograb;
};

extern arguments g_args;

enum ControllerType
{
    CONTROLLER_AI,
    CONTROLLER_KEYBOARD,
    CONTROLLER_KINECT
};

enum AiStrength {
    AI_WEAK,
    AI_STRONG
};

struct Settings
{
    AiStrength aiStrength;
    bool showBallGhosts;
    ControllerType controllerP1;
    ControllerType controllerP2;
    bool soundOn;
    bool previewOn;
};

extern Settings g_settings;

#endif /* end of include guard: GLOBAL_H_YO8PBGIO */
