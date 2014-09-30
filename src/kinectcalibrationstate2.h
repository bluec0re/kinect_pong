#ifndef KINECT_CALIBRATION_STATE2_H
#define KINECT_CALIBRATION_STATE2_H

#include "guistate.h"
#include "global.h"

#include <vector>
#include <utility>

class KinectCalibrationState2 : public GuiState
{
public:
    DECLARE_GAMESTATE_CLASS(KinectCalibrationState2);

    virtual void enter() override;
    virtual void exit() override;
    virtual bool pause() override;
    virtual void resume() override;

    bool mouseMoved( const OIS::MouseEvent &arg ) override;

    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;

protected:
    virtual void setupWindows();
    virtual void setupKinectWindow();
    bool handleKeyboardBtnClick(const CEGUI::EventArgs& args);
    virtual Ogre::Vector3 getCurrentPosition();
    virtual Ogre::Vector2 getCurrentRelPosition();
    virtual void selectHand();
    virtual void markArea();

    using GuiState::GuiState;

private:
    CEGUI::ProgressBar* _progress;
    CEGUI::Window* _status;
    CEGUI::Window* _target;
    CEGUI::Window* _markerArea;
    bool _tracked = false;
    bool _handSelected = false;
    bool _topLeft = false;
    bool _topRight = false;
    bool _bottomLeft = false;
    bool _bottomRight = false;
    bool _setuped = false;
    float _poseTimeout;
    std::vector<std::pair<Ogre::Vector3, Ogre::Vector3> > _positions;
    Ogre::Vector2 _lastPos;
};

#endif // KINECT_CALIBRATION_STATE2_H
