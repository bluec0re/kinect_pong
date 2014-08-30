#ifndef CEGUI_UTILS_H
#define CEGUI_UTILS_H


#include <OIS.h>
#include <CEGUI/CEGUI.h>

void InjectOISKey(bool bButtonDown, OIS::KeyEvent inKey);
void InjectOISMouseButton(bool bButtonDown, OIS::MouseButtonID inButton);
void InjectOISMouseMoved(const OIS::MouseState& state);

#endif // CEGUI_UTILS_H
