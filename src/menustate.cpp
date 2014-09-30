#include <OgreOggSound/OgreOggSound.h>
#include "menustate.h"
#include "cegui_utils.h"

using namespace CEGUI;


const float MenuState::s_firstStartDelay = 5.0f;
const float MenuState::s_secondStartDelay = 11.0f;
const float MenuState::s_loginDisplayStartDelay = 15.77f;

void MenuState::enter() {
    _guiContext = &CEGUI::System::getSingleton().getDefaultGUIContext();
    initialise(_guiContext);

    onEnteringSample();
}

void MenuState::exit() {
    _guiContext->getMouseCursor().hide();
    _guiContext->removeAllEvents();

    clearAll();
}

bool MenuState::pause() {
    _guiContext->getMouseCursor().hide();
    _guiContext->removeAllEvents();
    _guiContext->setDefaultFont(nullptr);

    OgreOggSound::OgreOggSoundManager* soundManager = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
    soundManager->getSound("menu_loop")->stop();

    clearAll();
    return true;
}

void MenuState::resume() {
    initialise(_guiContext);

    onEnteringSample();

    _guiContext->getMouseCursor().show();
}

bool MenuState::initialise(CEGUI::GUIContext* guiContext)
{
    using namespace CEGUI;

    _guiContext = guiContext;

    // load scheme and set up defaults
    SchemeManager::getSingleton().createFromFile("GameMenu.scheme");
    SchemeManager::getSingleton().createFromFile("Generic.scheme");
    _guiContext->getMouseCursor().setDefaultImage("GameMenuImages/MouseCursor");

    // load font and setup default if not loaded via scheme
    Font& defaultFont = FontManager::getSingleton().createFromFile("Jura-13.font");
    // Set default font for the gui context
    _guiContext->setDefaultFont(&defaultFont);

    FontManager::getSingleton().createFromFile("DejaVuSans-12.font");

    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();

    _root = winMgr.loadLayoutFromFile("GameMenu.layout");
    _guiContext->setRootWindow(_root);

    setupWindows();

    setupAnimations();

    _interactiveElementsWereInitiallyBlendedOut = false;
    _interactivePlanetElementsAreEnabled = false;
    _navigationIsEnabled = false;
    //_loginWasAccepted = false;
    _mouseIsHoveringNavi = false;

    _currentWriteFocus = WF_BotBar;

    return true;
}


void MenuState::setupAnimations()
{
    AnimationManager& animMgr = AnimationManager::getSingleton();
    animMgr.loadAnimationsFromXML("GameMenu.anims");

    CEGUI::Animation* startButtAnim = animMgr.getAnimation("StartButtonPulsating");
    CEGUI::AnimationInstance* startButtAnimInstance = animMgr.instantiateAnimation(startButtAnim);
    CEGUI::Window* startButtWindow = _root->getChild("InnerPartContainer/InsideStartClickArea/StartButtonImage");
    startButtAnimInstance->setTargetWindow(startButtWindow);
    startButtAnimInstance->start();

    CEGUI::Animation* insideImg1Anim = animMgr.getAnimation("InsideImage1Pulsating");
    CEGUI::AnimationInstance* insideImg1AnimInst = animMgr.instantiateAnimation(insideImg1Anim);
    CEGUI::Window* insideImg1 = _root->getChild("InnerPartContainer/InsideImage1");
    insideImg1AnimInst->setTargetWindow(insideImg1);
    insideImg1AnimInst->start();

    CEGUI::Animation* topBarAnim = animMgr.getAnimation("TopBarMoveInAnimation");
    _topBarAnimInst = animMgr.instantiateAnimation(topBarAnim);
    CEGUI::Window* topBarWindow = _root->getChild("TopBar");
    _topBarAnimInst->setTargetWindow(topBarWindow);

    CEGUI::Animation* botBarAnim = animMgr.getAnimation("BotBarMoveInAnimation");
    _botBarAnimInst = animMgr.instantiateAnimation(botBarAnim);
    CEGUI::Window* botBarWindow = _root->getChild("BotBar");
    _botBarAnimInst->setTargetWindow(botBarWindow);

    CEGUI::Animation* insideBlendInAnim = animMgr.getAnimation("InsideBlendIn");
    _insideBlendInAnimInst = animMgr.instantiateAnimation(insideBlendInAnim);
    CEGUI::Window* innerPartContainer = _root->getChild("InnerPartContainer");
    _insideBlendInAnimInst->setTargetWindow(innerPartContainer);

    CEGUI::Animation* insideImage3RotateIn = animMgr.getAnimation("InsideImage3RotateIn");
    _insideImage3RotateInInst = animMgr.instantiateAnimation(insideImage3RotateIn);
    CEGUI::Window* insideImage3 = _root->getChild("InnerPartContainer/OuterRingsContainer/InsideImage3");
    _insideImage3RotateInInst->setTargetWindow(insideImage3);

    CEGUI::Animation* insideImage4RotateIn = animMgr.getAnimation("InsideImage4RotateIn");
    _insideImage4RotateInInst = animMgr.instantiateAnimation(insideImage4RotateIn);
    CEGUI::Window* insideImage4 = _root->getChild("InnerPartContainer/OuterRingsContainer/InsideImage4");
    _insideImage4RotateInInst->setTargetWindow(insideImage4);

    CEGUI::Animation* insideImageRingsContainerSizeIn = animMgr.getAnimation("RingsContainerSizeIn");
    _insideImageRingsContainerSizeInInst = animMgr.instantiateAnimation(insideImageRingsContainerSizeIn);
    CEGUI::Window* insideImageContainer = _root->getChild("InnerPartContainer/OuterRingsContainer");
    _insideImageRingsContainerSizeInInst->setTargetWindow(insideImageContainer);

    CEGUI::Animation* buttonFadeInAnim = animMgr.getAnimation("ButtonFadeIn");
    _buttonFadeInAnimInst1 = animMgr.instantiateAnimation(buttonFadeInAnim);
    CEGUI::Window* window = _root->getChild("InnerButtonsContainer/ButtonOptions");
    _buttonFadeInAnimInst1->setTargetWindow(window);
    /*_buttonFadeInAnimInst2 = animMgr.instantiateAnimation(buttonFadeInAnim);
    window = _root->getChild("InnerButtonsContainer/ButtonLoad");
    _buttonFadeInAnimInst2->setTargetWindow(window);
    _buttonFadeInAnimInst3 = animMgr.instantiateAnimation(buttonFadeInAnim);
    window = _root->getChild("InnerButtonsContainer/ButtonSave");
    _buttonFadeInAnimInst3->setTargetWindow(window);
    _buttonFadeInAnimInst4 = animMgr.instantiateAnimation(buttonFadeInAnim);
    window = _root->getChild("InnerButtonsContainer/ButtonCharacters");
    _buttonFadeInAnimInst4->setTargetWindow(window);*/
    _buttonFadeInAnimInst5 = animMgr.instantiateAnimation(buttonFadeInAnim);
    window = _root->getChild("InnerButtonsContainer/ButtonQuit");
    _buttonFadeInAnimInst5->setTargetWindow(window);

    /*
    CEGUI::Animation* loginContainerMoveInAnim = animMgr.getAnimation("LoginMoveIn");
    _loginContainerMoveInInst = animMgr.instantiateAnimation(loginContainerMoveInAnim);
    _loginContainerMoveInInst->setTargetWindow(_loginContainer);

    CEGUI::Animation* loginContainerMoveOutAnim = animMgr.getAnimation("LoginMoveOut");
    _loginContainerMoveOutInst = animMgr.instantiateAnimation(loginContainerMoveOutAnim);
    _loginContainerMoveOutInst->setTargetWindow(_loginContainer);*/

/*
    CEGUI::Animation* naviButtonRightMoveInAnim = animMgr.getAnimation("NaviButtonRightMoveIn");
    _naviButtonRightMoveInInst = animMgr.instantiateAnimation(naviButtonRightMoveInAnim);
    window = _root->getChild("BotNavigationContainer/RightArrow");
    _naviButtonRightMoveInInst->setTargetWindow(window);

    CEGUI::Animation* naviButtonLeftMoveInAnim = animMgr.getAnimation("NaviButtonLeftMoveIn");
    _naviButtonLeftMoveInInst = animMgr.instantiateAnimation(naviButtonLeftMoveInAnim);
    window = _root->getChild("BotNavigationContainer/LeftArrow");
    _naviButtonLeftMoveInInst->setTargetWindow(window);

    CEGUI::Animation* naviBotMoveInAnim = animMgr.getAnimation("NaviBotCenterMoveIn");
    _naviBotMoveInInst = animMgr.instantiateAnimation(naviBotMoveInAnim);
    window = _root->getChild("BotNavigationContainer/NaviCenterContainer");
    _naviBotMoveInInst->setTargetWindow(window);*/

    CEGUI::Animation* blendInAnim = animMgr.getAnimation("BlendIn");
    _startButtonBlendInAnimInst = animMgr.instantiateAnimation(blendInAnim);
    _startButtonBlendInAnimInst->setTargetWindow(_startButtonClickArea);

    CEGUI::Animation* partialBlendOutAnim = animMgr.getAnimation("PartialBlendOut");

    /*window = _root->getChild("BotNavigationContainer");
    _naviPartialBlendOutInst = animMgr.instantiateAnimation(partialBlendOutAnim);
    _naviPartialBlendOutInst->setTargetWindow(window);
    _naviBlendInInst = animMgr.instantiateAnimation(blendInAnim);
    _naviBlendInInst->setTargetWindow(window);*/
    window = _root->getChild("InnerButtonsContainer");
    _centerButtonsPartialBlendOutInst = animMgr.instantiateAnimation(partialBlendOutAnim);
    _centerButtonsPartialBlendOutInst->setTargetWindow(window);
    _centerButtonsBlendInInst = animMgr.instantiateAnimation(blendInAnim);
    _centerButtonsBlendInInst->setTargetWindow(window);

/*
    window = _root->getChild("BotNavigationContainer/NaviCenterContainer/NavigationLabel");
    _navigationLabelPartialBlendOutInst = animMgr.instantiateAnimation(partialBlendOutAnim);
    _navigationLabelPartialBlendOutInst->setTargetWindow(window);
    _navigationLabelBlendInAnimInst = animMgr.instantiateAnimation(blendInAnim);
    _navigationLabelBlendInAnimInst->setTargetWindow(window);


    CEGUI::Animation* travelRotateInAnim = animMgr.getAnimation("TravelRotateIn");
    CEGUI::AnimationInstance* travelRotateInInst = animMgr.instantiateAnimation(travelRotateInAnim);
    travelRotateInInst->setTargetWindow(_navigationTravelIcon);
    CEGUI::Animation* travelRotateOutAnim = animMgr.getAnimation("TravelRotateOut");
    CEGUI::AnimationInstance* travelRotateOutInst = animMgr.instantiateAnimation(travelRotateOutAnim);
    travelRotateOutInst->setTargetWindow(_navigationTravelIcon);
*/
    CEGUI::Animation* blendOutAnim = animMgr.getAnimation("FullBlendOut");
    _botBarLabelBlendOutInst = animMgr.instantiateAnimation(blendOutAnim);
    _botBarLabelBlendOutInst->setTargetWindow(_botBarLabel);
/*
    CEGUI::Animation* loopRotateRightAnim = animMgr.getAnimation("LoopRotateRight");
    CEGUI::Animation* stopRotateAnim = animMgr.getAnimation("StopRotate");
    CEGUI::AnimationInstance* loopRotateRightAnimInst = animMgr.instantiateAnimation(loopRotateRightAnim);
    loopRotateRightAnimInst->setTargetWindow(_navigationSelectionIcon);
    CEGUI::AnimationInstance* loopRotateLeftAnimInst = animMgr.instantiateAnimation(stopRotateAnim);
    loopRotateLeftAnimInst->setTargetWindow(_navigationSelectionIcon);

    window = _root->getChild("BotNavigationContainer/RightArrow");
    CEGUI::Animation* naviArrowRightTwitch = animMgr.getAnimation("NaviArrowRightTwitch");
    CEGUI::AnimationInstance* naviArrowRightTwitchInst = animMgr.instantiateAnimation(naviArrowRightTwitch);
    naviArrowRightTwitchInst->setTargetWindow(window);
    CEGUI::Animation* naviArrowRightReturn = animMgr.getAnimation("NaviArrowRightReturn");
    CEGUI::AnimationInstance* naviArrowRightReturnInst = animMgr.instantiateAnimation(naviArrowRightReturn);
    naviArrowRightReturnInst->setTargetWindow(window);

    window = _root->getChild("BotNavigationContainer/LeftArrow");
    CEGUI::Animation* naviArrowLeftTwitch = animMgr.getAnimation("NaviArrowLeftTwitch");
    CEGUI::AnimationInstance* naviArrowLeftTwitchInst = animMgr.instantiateAnimation(naviArrowLeftTwitch);
    naviArrowLeftTwitchInst->setTargetWindow(window);
    CEGUI::Animation* naviArrowLeftReturn = animMgr.getAnimation("NaviArrowLeftReturn");
    CEGUI::AnimationInstance* naviArrowLeftReturnInst = animMgr.instantiateAnimation(naviArrowLeftReturn);
    naviArrowLeftReturnInst->setTargetWindow(window);*/

    setupPopupLinesAnimations();

    //setupSelectionIconAnimations();
}

void MenuState::onEnteringSample()
{
    //_navigationTravelIcon->setEnabled(false);

    _timeSinceStart = 0.0f;

    _interactiveElementsWereInitiallyBlendedOut = false;
    _interactivePlanetElementsAreEnabled = false;
    _loginWasAccepted = false;
    _startButtonClicked = false;

    _botBarLabel->setAlpha(1.0f);
//    _botNaviContainer->setAlpha(1.0f);

    _centerButtonsBlendInInst->setPosition(_centerButtonsBlendInInst->getDefinition()->getDuration());
    _centerButtonsBlendInInst->apply();

    _currentWriteFocus = WF_BotBar;

    //Reset labels and visibility
    _topBarLabel->setText("");
    _botBarLabel->setText("");


    /*_root->getChild("InnerButtonsContainer/PopupLinesSave")->setVisible(false);
    _root->getChild("InnerButtonsContainer/PopupLinesLoad")->setVisible(false);
    _root->getChild("InnerButtonsContainer/PopupLinesCharacters")->setVisible(false);*/
    _root->getChild("InnerButtonsContainer/PopupLinesQuit")->setVisible(false);
    _root->getChild("InnerButtonsContainer/PopupLinesOptions")->setVisible(false);

    resetAnimations();


    //_loginContainer->setVisible(false);
    _startButtonClickArea->setVisible(true);

    //Start the animations
    startEntranceAnimations();

    _startButtonClickArea->setAlpha(1.0f);
    _startButtonBlendInAnimInst->start();

    enableInteractivePlanetElements();

//    _navigationTravelIcon->setEnabled(true);

    _timeSinceLoginAccepted = 0.0f;
    _loginWasAccepted = true;
    //_botNaviContainer->setEnabled(true);
    _guiContext->getMouseCursor().hide();

    OgreOggSound::OgreOggSoundManager* soundManager = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
    soundManager->getSound("menu_loop")->play();
}

bool MenuState::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
    GuiState::frameRenderingQueued(evt);

    float timeSinceLastUpdate = evt.timeSinceLastFrame;
    _timeSinceStart += timeSinceLastUpdate;

    updateIntroText();

    if(_timeSinceStart > s_firstStartDelay) {
        _guiContext->getMouseCursor().show();
    }

    return true;
}

bool MenuState::handleInnerPartStartClickAreaClick(const CEGUI::EventArgs& args)
{/*
    if(!_interactivePlanetElementsAreEnabled)
        enableInteractivePlanetElements();

    _startButtonBlendInAnimInst->pause();
    _startButtonClickArea->setVisible(false);

    _startButtonClicked = true;*/

    GameState* gs;
    if(g_settings.useOldCalibration)
        gs = findByName("Kinect Calibration");
    else
        gs = findByName("Kinect Calibration2");

    if(g_settings.controllerP1 == CONTROLLER_KINECT && gs)
        pushGameState(gs);
    else
        pushGameState(findByName("Pong 2D"));

    return false;
}


bool MenuState::handleStartPopupLinesQuitDisplay(const CEGUI::EventArgs& args)
{
    popGameState();
    return false;
}


bool MenuState::handleStartPopupLinesOptionsDisplay(const CEGUI::EventArgs& args)
{
    pushGameState(findByName("Setting Menu"));
    return false;
}

bool MenuState::handleInnerButtonsLabelEntered(const CEGUI::EventArgs& args)
{
    const CEGUI::MouseEventArgs& mouseArgs = static_cast<const CEGUI::MouseEventArgs&>(args);

    CEGUI::Window* iconWindow = getIconWindowFromLabel(mouseArgs.window);

    CEGUI::EventArgs fireArgs;
    iconWindow->fireEvent("StartRotate", fireArgs);

    return false;
}

bool MenuState::handleInnerButtonsLabelLeft(const CEGUI::EventArgs& args)
{
    const CEGUI::MouseEventArgs& mouseArgs = static_cast<const CEGUI::MouseEventArgs&>(args);

    CEGUI::Window* iconWindow = getIconWindowFromLabel(mouseArgs.window);

    CEGUI::EventArgs fireArgs;
    iconWindow->fireEvent("StopRotate", fireArgs);

    return false;
}

void MenuState::stopStartPopupLinesAnimations()
{
    /*_popupLinesCharactersAnimInst->setPosition(_popupLinesLoadAnimInst->getDefinition()->getDuration());
    _popupLinesCharactersAnimInst->apply();
    _popupLinesCharactersAnimInst->pause();
    _popupLinesLoadAnimInst->setPosition(_popupLinesLoadAnimInst->getDefinition()->getDuration());
    _popupLinesLoadAnimInst->apply();
    _popupLinesLoadAnimInst->pause();
    _popupLinesSaveAnimInst->setPosition(_popupLinesSaveAnimInst->getDefinition()->getDuration());
    _popupLinesSaveAnimInst->apply();
    _popupLinesSaveAnimInst->pause();*/
    _popupLinesQuitAnimInst->setPosition(_popupLinesQuitAnimInst->getDefinition()->getDuration());
    _popupLinesQuitAnimInst->apply();
    _popupLinesQuitAnimInst->pause();
    _popupLinesOptionsAnimInst->setPosition(_popupLinesOptionsAnimInst->getDefinition()->getDuration());
    _popupLinesOptionsAnimInst->apply();
    _popupLinesOptionsAnimInst->pause();
/*
    _root->getChild("InnerButtonsContainer/PopupLinesSave")->setVisible(false);
    _root->getChild("InnerButtonsContainer/PopupLinesLoad")->setVisible(false);
    _root->getChild("InnerButtonsContainer/PopupLinesCharacters")->setVisible(false);*/
    _root->getChild("InnerButtonsContainer/PopupLinesOptions")->setVisible(false);
    _root->getChild("InnerButtonsContainer/PopupLinesQuit")->setVisible(false);

}


void MenuState::setupWindows()
{
    _botBarLabel = _root->getChild("BotBar/BotBarLabel");
    _topBarLabel = _root->getChild("TopBar/TopBarLabel");

  //  _loginContainer = _root->getChild("LoginContainer");

    //setupNaviArrowWindows();

    setupButtonClickHandlers();
/*
    _botNaviContainer = _root->getChild("BotNavigationContainer");
    _botNaviCenter = _root->getChild("BotNavigationContainer/NaviCenterContainer");
*/
/*
    _loginAcceptButton = _loginContainer->getChild("AcceptButton");
    _loginAcceptButton->subscribeEvent(CEGUI::Window::EventMouseClick, Event::Subscriber(&MenuState::handleLoginAcceptButtonClicked, this));
*/
    _startButtonClickArea = _root->getChild("InnerPartContainer/InsideStartClickArea");
    _startButtonClickArea->subscribeEvent(CEGUI::Window::EventMouseClick, Event::Subscriber(&MenuState::handleInnerPartStartClickAreaClick, this));
/*
    _navigationTravelIcon = _root->getChild("BotNavigationContainer/NaviCenterContainer/NavigationIcon");
    _navigationSelectionIcon = _root->getChild("BotNavigationContainer/NaviCenterContainer/NaviBotSelectionIcon");

    _navigationSelectionIcon->subscribeEvent(CEGUI::AnimationInstance::EventAnimationStarted, Event::Subscriber(&MenuState::handleNaviSelectionIconAnimStart, this));

    setupNaviIconAnimationEventHandlers();
*/
    //setupInnerButtonsSubOptionsLabels();

}

void MenuState::disableInteractivePlanetElements()
{
    _centerButtonsBlendInInst->pause();
    _centerButtonsPartialBlendOutInst->start();


    _interactiveElementsWereInitiallyBlendedOut = true;
}

void MenuState::enableInteractivePlanetElements()
{
    _centerButtonsPartialBlendOutInst->pause();
    _centerButtonsBlendInInst->start();

    _interactivePlanetElementsAreEnabled = true;
}

void MenuState::updateIntroText()
{
    static const CEGUI::String firstPart = "Connection established...";
    static const CEGUI::String secondPart = "Warning! User Authentication required!";

    CEGUI::String finalText;

    int firstPartTypeProgress = static_cast<int>((_timeSinceStart - s_firstStartDelay) / 0.08f);
    if(firstPartTypeProgress > 0)
        finalText += firstPart.substr(0, std::min<unsigned int>(firstPart.length(), firstPartTypeProgress));
/*
    int secondPartTypeProgress = static_cast<int>((_timeSinceStart - s_secondStartDelay) / 0.08f);
    if(secondPartTypeProgress > 0)
        finalText += "\n" + secondPart.substr(0, std::min<unsigned int>(secondPart.length(), secondPartTypeProgress));
        */

    finalText += "[font='DejaVuSans-12']";

    static double blinkStartDelay = 3.6f;
    double blinkPeriod = 0.8;
    double blinkTime = std::modf(static_cast<double>(_timeSinceStart), &blinkPeriod);
    if(blinkTime > 0.55 || _timeSinceStart < blinkStartDelay || _currentWriteFocus != WF_BotBar)
        finalText += "[colour='00000000']";

    finalText += reinterpret_cast<const encoded_char*>("❚");

    _botBarLabel->setText(finalText);
}

void MenuState::startEntranceAnimations()
{
    _topBarAnimInst->start();
    _botBarAnimInst->start();
    _insideBlendInAnimInst->start();
    _insideImage3RotateInInst->start();
    _insideImage4RotateInInst->start();
    _insideImageRingsContainerSizeInInst->start();
    _buttonFadeInAnimInst1->start();
    /*_buttonFadeInAnimInst2->start();
    _buttonFadeInAnimInst3->start();
    _buttonFadeInAnimInst4->start();*/
    _buttonFadeInAnimInst5->start();/*
    _naviButtonRightMoveInInst->start();
    _naviButtonLeftMoveInInst->start();
    _naviBotMoveInInst->start();*/
}

void MenuState::resetAnimations()
{
    //Set animations to end and apply their changes to the windows
    //For every animation instance that uses source properties
    //so the values will be taken correctly after starting them again
    _buttonFadeInAnimInst1->setPosition(_buttonFadeInAnimInst1->getDefinition()->getDuration());
    _buttonFadeInAnimInst1->apply();
    /*_buttonFadeInAnimInst2->setPosition(_buttonFadeInAnimInst2->getDefinition()->getDuration());
    _buttonFadeInAnimInst2->apply();
    _buttonFadeInAnimInst3->setPosition(_buttonFadeInAnimInst3->getDefinition()->getDuration());
    _buttonFadeInAnimInst3->apply();
    _buttonFadeInAnimInst4->setPosition(_buttonFadeInAnimInst4->getDefinition()->getDuration());
    _buttonFadeInAnimInst4->apply();*/
    _buttonFadeInAnimInst5->setPosition(_buttonFadeInAnimInst5->getDefinition()->getDuration());
    _buttonFadeInAnimInst5->apply();/*
    _loginContainerMoveInInst->setPosition(_loginContainerMoveInInst->getDefinition()->getDuration());
    _loginContainerMoveInInst->apply();
    _naviButtonRightMoveInInst->setPosition(_naviButtonRightMoveInInst->getDefinition()->getDuration());
    _naviButtonRightMoveInInst->apply();
    _naviButtonLeftMoveInInst->setPosition(_naviButtonLeftMoveInInst->getDefinition()->getDuration());
    _naviButtonLeftMoveInInst->apply();
    _naviBotMoveInInst->setPosition(_naviBotMoveInInst->getDefinition()->getDuration());
    _naviBotMoveInInst->apply();*/
}

void MenuState::setupButtonClickHandlers()
{
    /*CEGUI::Window* buttonSave = _root->getChild("InnerButtonsContainer/ButtonSave");
    buttonSave->subscribeEvent(CEGUI::Window::EventMouseClick, Event::Subscriber(&MenuState::handleStartPopupLinesSaveDisplay, this));
    CEGUI::Window* buttonLoad = _root->getChild("InnerButtonsContainer/ButtonLoad");
    buttonLoad->subscribeEvent(CEGUI::Window::EventMouseClick, Event::Subscriber(&MenuState::handleStartPopupLinesLoadDisplay, this));
    CEGUI::Window* buttonCharacters = _root->getChild("InnerButtonsContainer/ButtonCharacters");
    buttonCharacters->subscribeEvent(CEGUI::Window::EventMouseClick, Event::Subscriber(&MenuState::handleStartPopupLinesCharactersDisplay, this));*/
    CEGUI::Window* buttonOptions = _root->getChild("InnerButtonsContainer/ButtonOptions");
    buttonOptions->subscribeEvent(CEGUI::Window::EventMouseClick, Event::Subscriber(&MenuState::handleStartPopupLinesOptionsDisplay, this));
    CEGUI::Window* buttonQuit = _root->getChild("InnerButtonsContainer/ButtonQuit");
    buttonQuit->subscribeEvent(CEGUI::Window::EventMouseClick, Event::Subscriber(&MenuState::handleStartPopupLinesQuitDisplay, this));
}

CEGUI::Window* MenuState::getIconWindowFromLabel(CEGUI::Window* window)
{
    const CEGUI::String windowPrefix = "Label";
    const CEGUI::String iconPostfix = "SelectionIcon";

    CEGUI::String windowName = window->getName();

    windowName = windowName.substr(windowPrefix.length());
    return window->getParent()->getParent()->getChild(windowName + iconPostfix);
}

void MenuState::setupPopupLinesAnimations()
{
    CEGUI::Window* window;
    CEGUI::AnimationManager& animMgr = CEGUI::AnimationManager::getSingleton();

    CEGUI::Animation* sizeGrowth = animMgr.getAnimation("SizeGrowth");
    /*window = _root->getChild("InnerButtonsContainer/PopupLinesSave");
    _popupLinesSaveAnimInst = animMgr.instantiateAnimation(sizeGrowth);
    _popupLinesSaveAnimInst->setTarget(window);
    window = _root->getChild("InnerButtonsContainer/PopupLinesLoad");
    _popupLinesLoadAnimInst = animMgr.instantiateAnimation(sizeGrowth);
    _popupLinesLoadAnimInst->setTarget(window);
    window = _root->getChild("InnerButtonsContainer/PopupLinesCharacters");
    _popupLinesCharactersAnimInst = animMgr.instantiateAnimation(sizeGrowth);
    _popupLinesCharactersAnimInst->setTarget(window);*/
    window = _root->getChild("InnerButtonsContainer/PopupLinesOptions");
    _popupLinesOptionsAnimInst = animMgr.instantiateAnimation(sizeGrowth);
    _popupLinesOptionsAnimInst->setTarget(window);
    window = _root->getChild("InnerButtonsContainer/PopupLinesQuit");
    _popupLinesQuitAnimInst = animMgr.instantiateAnimation(sizeGrowth);
    _popupLinesQuitAnimInst->setTarget(window);
}
