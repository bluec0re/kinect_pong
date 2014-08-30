#include "menustate.h"
#include "cegui_utils.h"

void MenuState::enter() {
}
void MenuState::exit() {
}

bool MenuState::keyPressed(const OIS::KeyEvent &arg) {
    InjectOISKey(true, arg);
}

bool MenuState::keyReleased(const OIS::KeyEvent &arg) {
    InjectOISKey(false, arg);
}

bool MenuState::mouseMoved( const OIS::MouseEvent &arg ) {
    InjectOISMouseMoved(arg.state);
}

bool MenuState::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
    InjectOISMouseButton(true, id);
}

bool MenuState::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
    InjectOISMouseButton(false, id);
}

bool MenuState::frameRenderingQueued(const Ogre::FrameEvent& evt) {
    CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);
}
