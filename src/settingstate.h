#ifndef __SettingState_H_
#define __SettingState_H_

#include "guistate.h"

class SettingState : public GuiState {
public:
    DECLARE_GAMESTATE_CLASS(SettingState);

    virtual void enter() override;
    virtual void exit() override;

    static void loadSettings();
    static void saveSettings();

protected:
    virtual void setupWindows();
    virtual bool handleSaveClick(const CEGUI::EventArgs& args);
    virtual bool handleCancelClick(const CEGUI::EventArgs& args);
    virtual bool handleP1Selected(const CEGUI::EventArgs& args);
    virtual bool handleP2Selected(const CEGUI::EventArgs& args);
};


#endif //__SettingState_H_
