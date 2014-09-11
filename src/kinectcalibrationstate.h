#ifndef KINECT_CALIBRATION_STATE_H
#define KINECT_CALIBRATION_STATE_H

#include "guistate.h"
#include "global.h"


class KinectCalibrationState : public GuiState
{
public:
    using GuiState::GuiState;
    DECLARE_GAMESTATE_CLASS(KinectCalibrationState);

    virtual void enter() override;
    virtual void exit() override;
    virtual bool pause() override;
    virtual void resume() override;

    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
protected:
    virtual void setupWindows();
    virtual void setupKinectWindow();
    bool handleKeyboardBtnClick(const CEGUI::EventArgs& args);

private:
    CEGUI::ProgressBar* _progress;
    bool _tracked = false;
    bool _calibrated = false;
    float _tPoseTimeout;
};

#endif // KINECT_CALIBRATION_STATE_H