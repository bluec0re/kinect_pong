#include "cegui_utils.h"

void InjectOISKey(bool bButtonDown, OIS::KeyEvent inKey)
{
    if (bButtonDown)
    {
        CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown((CEGUI::Key::Scan)inKey.key);
        CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(inKey.text);
    }
    else
    {
        CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)inKey.key);
    }
}

void InjectOISMouseButton(bool bButtonDown, OIS::MouseButtonID inButton)
{
    if (bButtonDown == true)
    {
        switch (inButton)
        {
        case OIS::MB_Left:
            CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::LeftButton);
            break;
        case OIS::MB_Middle:
            CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::MiddleButton);
            break;
        case OIS::MB_Right:
            CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::RightButton);
            break;
        case OIS::MB_Button3:
            CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::X1Button);
            break;
        case OIS::MB_Button4:
            CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::X2Button);
            break;
        default:
            break;

        }
    }
    else // bButtonDown = false
    {
        switch (inButton)
        {
        case OIS::MB_Left:
            CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(CEGUI::LeftButton);
            break;
        case OIS::MB_Middle:
            CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(CEGUI::MiddleButton);
            break;
        case OIS::MB_Right:
            CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(CEGUI::RightButton);
            break;
        case OIS::MB_Button3:
            CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(CEGUI::X1Button);
            break;
        case OIS::MB_Button4:
            CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(CEGUI::X2Button);
            break;
        default:
            break;
        }
    }

}

void InjectOISMouseMoved(const OIS::MouseState& state)
{
    CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(state.X.rel, state.Y.rel);
    if(state.Z.rel)
        CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseWheelChange(state.Z.rel / 120.f);
}
