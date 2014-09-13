#ifndef KINECT_CALIBRATION_STATE_H
#define KINECT_CALIBRATION_STATE_H

#include "guistate.h"
#include "global.h"

#include <vector>
#include <utility>

class KinectCalibrationState : public GuiState
{
public:
    DECLARE_GAMESTATE_CLASS(KinectCalibrationState);

    virtual void enter() override;
    virtual void exit() override;
    virtual bool pause() override;
    virtual void resume() override;

    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;

protected:
    virtual void setupWindows();
    virtual void setupKinectWindow();
    bool handleKeyboardBtnClick(const CEGUI::EventArgs& args);
    virtual void updateMarkers();

    using GuiState::GuiState;

private:
    CEGUI::ProgressBar* _progress;
    CEGUI::Window* _manual;
    CEGUI::Window* _target;
    bool _tracked = false;
    bool _calibrated = false;
    float _tPoseTimeout;
    std::vector<std::pair<Ogre::Vector3, Ogre::Vector3> > _positions;
};

#endif // KINECT_CALIBRATION_STATE_H