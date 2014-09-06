#include "guistate.h"
#include "cegui_utils.h"

bool GuiState::keyPressed(const OIS::KeyEvent &arg) {
    InjectOISKey(true, arg);

    return true;
}

bool GuiState::keyReleased(const OIS::KeyEvent &arg) {
    InjectOISKey(false, arg);

    return true;
}

bool GuiState::mouseMoved( const OIS::MouseEvent &arg ) {
    InjectOISMouseMoved(arg.state);

    return true;
}

bool GuiState::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
    InjectOISMouseButton(true, id);

    return true;
}

bool GuiState::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
    InjectOISMouseButton(false, id);

    return true;
}

bool GuiState::frameRenderingQueued(const Ogre::FrameEvent& evt) {
    CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);

    return true;
}
