#ifndef GLOBAL_H_YO8PBGIO
#define GLOBAL_H_YO8PBGIO


#include <vector>
#include <Ogre.h>
#include <OIS.h>
//#include <CEGUI.h>

class GameStateManager;

typedef struct
{
    Ogre::Root *ogre;
    Ogre::RenderWindow *rwindow;
    Ogre::SceneManager *sceneMgr;
    OIS::InputManager *InputMgr;
    OIS::Keyboard *keyboard;
    OIS::Mouse *mouse;
    //CEGUI::OgreCEGUIRenderer *GUIRenderer;
    //CEGUI::System *GUISystem;
} device_info;

#endif /* end of include guard: GLOBAL_H_YO8PBGIO */
