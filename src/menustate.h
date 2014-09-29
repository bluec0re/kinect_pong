#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "guistate.h"

enum WriteFocus
{
    WF_TopBar,
    WF_BotBar,

    WF_Count
};

class MenuState : public GuiState
{
public:
    DECLARE_GAMESTATE_CLASS(MenuState);


    virtual void enter() override;
    virtual void exit() override;
    virtual bool pause() override;
    virtual void resume() override;

    virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt) override;

protected:
    using GuiState::GuiState;

    bool initialise(CEGUI::GUIContext* guiContext);
    void setupAnimations();
    void onEnteringSample();
    bool handleInnerPartStartClickAreaClick(const CEGUI::EventArgs& args);
    bool handleCheckIfNaviIconAnimationNeedsChange(const CEGUI::EventArgs& args);
    bool handleNaviSelectionIconAnimStart(const CEGUI::EventArgs& args);
    bool handleStartPopupLinesQuitDisplay(const CEGUI::EventArgs& args);
    bool handleStartPopupLinesOptionsDisplay(const CEGUI::EventArgs& args);
    bool handleInnerButtonsLabelEntered(const CEGUI::EventArgs& args);
    bool handleInnerButtonsLabelLeft(const CEGUI::EventArgs& args);
    void stopStartPopupLinesAnimations();
    void setupWindows();
    void disableInteractivePlanetElements();
    void enableInteractivePlanetElements();
    void updateIntroText();
    void startEntranceAnimations();
    void resetAnimations();
    void setupButtonClickHandlers();
    void setupInnerButtonsSubOptionsLabels();
    CEGUI::Window* getIconWindowFromLabel(CEGUI::Window* window);
    void setupPopupLinesAnimations();


protected:
    CEGUI::Window* _root;
    CEGUI::GUIContext* _guiContext;

    static const float s_firstStartDelay;
    static const float s_secondStartDelay;
    static const float s_loginDisplayStartDelay;

    //WriteFocus _currentWriteFocus;
    float _timeSinceStart;

    float _timeSinceLoginAccepted;

    bool _interactiveElementsWereInitiallyBlendedOut;
    bool _interactivePlanetElementsAreEnabled;
    bool _navigationIsEnabled;
    bool _loginWasAccepted;
    bool _mouseIsHoveringNavi;
    bool _startButtonClicked;

    WriteFocus _currentWriteFocus;

    CEGUI::String _userName;
    CEGUI::AnimationInstance* _topBarAnimInst;
    CEGUI::AnimationInstance* _botBarAnimInst;
    CEGUI::AnimationInstance* _insideBlendInAnimInst;
    CEGUI::AnimationInstance* _insideImage3RotateInInst;
    CEGUI::AnimationInstance* _insideImage4RotateInInst;
    CEGUI::AnimationInstance* _insideImageRingsContainerSizeInInst;

    CEGUI::AnimationInstance* _buttonFadeInAnimInst1;
    CEGUI::AnimationInstance* _buttonFadeInAnimInst5;

    CEGUI::AnimationInstance* _startButtonBlendInAnimInst;

    CEGUI::AnimationInstance* _centerButtonsPartialBlendOutInst;
    CEGUI::AnimationInstance* _centerButtonsBlendInInst;

    CEGUI::AnimationInstance* _botBarLabelBlendOutInst;

    CEGUI::AnimationInstance* _popupLinesQuitAnimInst;
    CEGUI::AnimationInstance* _popupLinesOptionsAnimInst;

    CEGUI::Window* _topBarLabel;
    CEGUI::Window* _botBarLabel;
    CEGUI::Window* _startButtonClickArea;
};

#endif // MENUSTATE_H
