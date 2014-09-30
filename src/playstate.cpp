#include "global.h"
#include "playstate.h"
#include "gamestatemanager.h"

#include "keyboardplayer.h"
#include "aiplayer.h"

#include <OgreOggSound/OgreOggSound.h>

#ifdef HAVE_OPENNI2
#include "kinect.h"
#include "kinectplayer.h"
#endif

PlayState::PlayState() : GuiState(), _paused(false), _endScreenTimeout(0), _ended(false) {
}

/**
 * create the box around the game field
 *
 * @brief createBox
 * @param red
 * @param green
 * @param blue
 * @param clipPlane  lines behind this plane would not be drawn
 */
void PlayState::createBox(Ogre::Real red, Ogre::Real green, Ogre::Real blue, const Ogre::Plane& clipPlane) {
    Ogre::ManualObject* myManualObject  = mDevice->sceneMgr->createManualObject("box");
    Ogre::SceneNode* myManualObjectNode = mDevice->sceneMgr->getRootSceneNode()->createChildSceneNode("box_node");

    // NOTE: The second parameter to the create method is the resource group the material will be added to.
    // If the group you name does not exist (in your resources.cfg file) the library will assert() and your program will crash
    Ogre::MaterialPtr myManualObjectMaterial = Ogre::MaterialManager::getSingleton().create("boxMaterial","General");
    myManualObjectMaterial->setReceiveShadows(false);
    myManualObjectMaterial->getTechnique(0)->setLightingEnabled(true);
    myManualObjectMaterial->getTechnique(0)->getPass(0)->setDiffuse(red, green, blue,0);
    myManualObjectMaterial->getTechnique(0)->getPass(0)->setAmbient(0,0,1);
    myManualObjectMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(red,green,blue);
    //myManualObjectMaterial->;  // dispose pointer, not the material


    myManualObject->begin("boxMaterial", Ogre::RenderOperation::OT_LINE_LIST);
    myManualObject->position(-MAP_X, -MAP_Y, -MAP_Z); // 0
    myManualObject->position(-MAP_X, -MAP_Y,  MAP_Z);
    myManualObject->position(-MAP_X,  MAP_Y,  MAP_Z);
    myManualObject->position(-MAP_X,  MAP_Y, -MAP_Z); // 3
    myManualObject->position( MAP_X, -MAP_Y, -MAP_Z);
    myManualObject->position( MAP_X, -MAP_Y,  MAP_Z); // 5
    myManualObject->position( MAP_X,  MAP_Y,  MAP_Z);
    myManualObject->position( MAP_X,  MAP_Y, -MAP_Z); // 7

    myManualObject->index(0);
    myManualObject->index(1);

    myManualObject->index(1);
    myManualObject->index(2);

    myManualObject->index(2);
    myManualObject->index(3);

    myManualObject->index(3);
    myManualObject->index(0);

    myManualObject->index(0);
    myManualObject->index(4);

    myManualObject->index(4);
    myManualObject->index(5);

    myManualObject->index(5);
    myManualObject->index(6);

    myManualObject->index(6);
    myManualObject->index(7);

    myManualObject->index(7);
    myManualObject->index(4);

    myManualObject->index(7);
    myManualObject->index(3);

    myManualObject->index(6);
    myManualObject->index(2);

    myManualObject->index(1);
    myManualObject->index(5);
    // etc
    myManualObject->end();

    myManualObjectNode->attachObject(myManualObject);

    createSubBox(red*0.5, green*0.5, blue*0.5, clipPlane, myManualObjectNode);
}

/**
 * draws the subdivisions of the box
 *
 * @brief createSubBox
 * @param red
 * @param green
 * @param blue
 * @param clipPlane
 * @param parent
 */
void PlayState::createSubBox(Ogre::Real red, Ogre::Real green, Ogre::Real blue, const Ogre::Plane& clipPlane, Ogre::SceneNode* parent) {
    Ogre::ManualObject* myManualObject  = mDevice->sceneMgr->createManualObject("box_sub");
    Ogre::SceneNode* myManualObjectNode = parent->createChildSceneNode("box_subnode");

    // NOTE: The second parameter to the create method is the resource group the material will be added to.
    // If the group you name does not exist (in your resources.cfg file) the library will assert() and your program will crash
    Ogre::MaterialPtr myManualObjectMaterial = Ogre::MaterialManager::getSingleton().create("boxMaterial_subnode","General");
    myManualObjectMaterial->setReceiveShadows(false);
    myManualObjectMaterial->getTechnique(0)->setLightingEnabled(true);
    myManualObjectMaterial->getTechnique(0)->getPass(0)->setDiffuse(red, green, blue, 0);
    myManualObjectMaterial->getTechnique(0)->getPass(0)->setAmbient(red, green, blue);
    myManualObjectMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(red,green,blue);
    //myManualObjectMaterial->;  // dispose pointer, not the material

    std::vector<Ogre::Vector3> positions;

    myManualObject->begin("boxMaterial_subnode", Ogre::RenderOperation::OT_LINE_LIST);

    // create vertices
    int loops = 4;
    int step = MAP_SIZE*2/(loops+1);

    // xy
    for(int l = 1; l <= loops; l++) {
        for(int a = -1; a < 2; a+=2) {
            for(int b = -1; b < 2; b+=2) {
                Ogre::Vector3 pos(MAP_X*a, MAP_Y*b*a, -MAP_Z + l*step);
                myManualObject->position(pos);
                positions.push_back(pos);
            }
        }
    }

    // xz
    for(int l = 1; l <= loops; l++) {
        for(int a = -1; a < 2; a+=2) {
            for(int b = -1; b < 2; b+=2) {
                Ogre::Vector3 pos(MAP_X*b*a, -MAP_Y + l*step, MAP_Z*a);
                myManualObject->position(pos);
                positions.push_back(pos);
            }
        }
    }

    // yz
    for(int l = 1; l <= loops*2+1; l++) {
        for(int a = -1; a < 2; a+=2) {
            for(int b = -1; b < 2; b+=2) {
                Ogre::Vector3 pos(-MAP_X + l*step, MAP_Y*b*a, MAP_Z*a);
                myManualObject->position(pos);
                positions.push_back(pos);
            }
        }
    }

    // connect the vertices
    for(int l = 0; l < loops*4+1; l++) {
        int i = l*4;
        for(int j = 0; j < 4; j++) {
            int a = i+j;
            int b = i+((j+1)%4);

            Ogre::Vector3& pa = positions.at(a);
            Ogre::Vector3& pb = positions.at(b);

            // test if the line lies behind the plane
            Ogre::Plane::Side sidea = clipPlane.getSide(pa);
            if(sidea != Ogre::Plane::NEGATIVE_SIDE ||
               sidea != clipPlane.getSide(pb)) {
                myManualObject->index(a);
                myManualObject->index(b);
            }
        }
    }
    // etc
    myManualObject->end();

    myManualObjectNode->attachObject(myManualObject);
}

void PlayState::enter() {
    _paddles.clear();
    _balls.clear();
    _players.clear();
    _playerLabels.clear();
    _scoreLabels.clear();
    _ended = false;
    _paused = false;
    _endScreenTimeout = 0.0;

    Ogre::Light* pointLight = mDevice->sceneMgr->createLight("pointLight");
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setPosition(Ogre::Vector3::ZERO);
    pointLight->setDiffuseColour(1.0, 1.0, 1.0);
    pointLight->setSpecularColour(1.0, 1.0, 1.0);

    {
        PaddlePtr p = addPaddle(0xFF0000, "Player 1", -MAP_BBOX_X);
        addPlayer(g_settings.controllerP1, p);
    }

    {        
        PaddlePtr p = addPaddle(0x0000FF, "Player 2", MAP_BBOX_X);
        addPlayer(g_settings.controllerP2, p);
    }

    {
        BallPtr ball = addBall();

        resetBall(ball);
    }

    CEGUI::Logger::getSingleton().logEvent("Entering Playstate");

    // load font and setup default if not loaded via scheme
    CEGUI::Font& defaultFont = CEGUI::FontManager::getSingleton().createFromFile("Jura-13.font");
    // Set default font for the gui context
    CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont(&defaultFont);

    CEGUI::FontManager::getSingleton().createFromFile("DejaVuSans-12.font");

    OgreOggSound::OgreOggSoundManager* soundManager = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
    soundManager->getSound("game_loop")->play();

    setupWindows();
}

void PlayState::setupWindows() {
    using namespace CEGUI;

    WindowManager& wmgr = WindowManager::getSingleton();
    GUIContext& guiContext = System::getSingleton().getDefaultGUIContext();

    SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");

    Window* root = wmgr.createWindow("DefaultWindow");
    guiContext.setRootWindow(root);


#ifdef HAVE_OPENNI2
    if(Kinect::getInstance()->isConnected() && g_settings.previewOn) {
        Logger::getSingleton().logEvent("Creating Kinect Window");
        Kinect::getInstance()->update();
        // preview

        Texture& cetex = mDevice->guiRenderer->createTexture("depthMap", Kinect::getInstance()->getDepthImage());
        /*
        Ogre::Image img;
        Kinect::getInstance()->getDepthImage()->convertToImage(img);
        img.save("/tmp/test.png");
        */
        const Rectf rect(Vector2f(0.0f, 0.0f), cetex.getOriginalDataSize());
        BasicImage* image = dynamic_cast<BasicImage*>(&ImageManager::getSingleton().create("BasicImage", "KinectImage"));
           image->setTexture(&cetex);
           image->setArea(rect);
           image->setAutoScaled(ASM_Both);

        Window *si = wmgr.createWindow("TaharezLook/StaticImage", "KinectWindow");
        si->setSize(USize(UDim(0.25f, 0),
                          UDim(0.25f, 0)));
        si->setPosition(UVector2(UDim(0.75f, 0),
                                 UDim(0.0f, 0)));
        si->setProperty("Image", "KinectImage");
        root->addChild(si);
        Logger::getSingleton().logEvent("Kinect Window created");
    }
#endif

    // FIXME: leads to crash when switched between modes
    Window* player1Label = wmgr.createWindow("TaharezLook/Label", "Player1Label");
    player1Label->setPosition(UVector2(UDim(0, 0),
                                       UDim(0, 0)));
    player1Label->setSize(USize(UDim(0.2f, 0),
                                UDim(0.1f, 0)));
    player1Label->setText("Player 1");
    root->addChild(player1Label);
    _playerLabels.push_back(player1Label);


    Window* player2Label = wmgr.createWindow("TaharezLook/Label", "Player2Label");
    player2Label->setPosition(UVector2(UDim(0.8f, 0),
                                       UDim(0, 0)));
    player2Label->setSize(USize(UDim(0.2f, 0),
                                UDim(0.1f, 0)));
    player2Label->setText("Player 2");
    root->addChild(player2Label);
    _playerLabels.push_back(player2Label);

    Window* player1Score = wmgr.createWindow("TaharezLook/Label", "Player1Score");
    player1Score->setPosition(UVector2(UDim(0.3f, 0),
                                       UDim(0, 0)));
    player1Score->setSize(USize(UDim(0.1f, 0),
                                UDim(0.1f, 0)));
    player1Score->setText("3");
    root->addChild(player1Score);
    _scoreLabels.push_back(player1Score);


    Window* player2Score = wmgr.createWindow("TaharezLook/Label", "Player2Score");
    player2Score->setPosition(UVector2(UDim(0.6f, 0),
                                       UDim(0, 0)));
    player2Score->setSize(USize(UDim(0.1f, 0),
                                UDim(0.1f, 0)));
    player2Score->setText("3");
    root->addChild(player2Score);
    _scoreLabels.push_back(player2Score);

    Ogre::LogManager::getSingleton().logMessage("Entered Playstate");
}

void PlayState::updateScore() {
    using namespace CEGUI;

    WindowManager& wmgr = WindowManager::getSingleton();

    for(size_t i = 0; i < _players.size(); i++) {
        const PlayerPtr& player = _players[i];
        Window* label = _scoreLabels[i];
        Ogre::StringStream ss;
        ss << player->getLives();
        label->setText(ss.str());

        label = _playerLabels[i];
        if(player->isReady()) {
            label->setText(player->getName());
        } else {
            label->setText("<not ready>");
        }
    }
}

void PlayState::exit() {
    mDevice->sceneMgr->destroyAllManualObjects();
    mDevice->sceneMgr->getRootSceneNode()->removeAndDestroyAllChildren();
    mDevice->sceneMgr->destroyAllLights();
    /*mDevice->sceneMgr->destroySceneNode("box_node");
    mDevice->sceneMgr->destroySceneNode("box_subnode");*/

    clearAll();

    OgreOggSound::OgreOggSoundManager* soundManager = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
    soundManager->getSound("game_loop")->stop();

    CEGUI::Logger::getSingleton().logEvent("Leaving Playstate");
    Ogre::LogManager::getSingleton().logMessage("Leaving Playstate");
}

bool PlayState::keyReleased(const OIS::KeyEvent &arg) {
    if(!GuiState::keyReleased(arg))
        return false;

    if(arg.key == OIS::KC_ESCAPE) {
        parent->popGameState();
    } else if(arg.key == OIS::KC_TAB) {
        GameState* next = parent->findByName("Pong 2D");
        if(next != this)
            parent->changeGameState(next);
        else {
            next = parent->findByName("Pong 3D");
            if(next != this)
                parent->changeGameState(next);
        }
    } else if(arg.key == OIS::KC_P) {
        _paused = !_paused;
    } else if(arg.key == OIS::KC_B) {
        addBall();
    }
    return true;
}

BallPtr PlayState::addBall() {
    BallPtr ball(new Ball(0xFF00FF00, 0xFFBF6900, false));
    //BallPtr ball(new Ball(0xFF0000FF, 0xFFFF0000));
    ball->setSceneManager(mDevice->sceneMgr);
    ball->create();
    ball->setAccel(getRandomAccel());
    ball->enableAccel(false);
    ball->setSpeed(getRandomSpeed());
    _balls.push_back(ball);

    return ball;
}

PaddlePtr PlayState::addPaddle(int color, const Ogre::String& name, const Ogre::Real& pos) {
    PaddlePtr p(new Paddle(color, name));
    p->setSceneManager(mDevice->sceneMgr);
    p->create();
    p->setPosition(Ogre::Vector3(pos, 0, 0));
    _paddles.push_back(p);

    return p;
}

PlayerPtr PlayState::addPlayer(const ControllerType& controller, const PaddlePtr& p) {
    PlayerPtr player;
    switch(controller) {
        case CONTROLLER_KINECT: {
#ifdef HAVE_OPENNI2
            if(Kinect::getInstance()->isConnected()) {
                player.reset(new KinectPlayer(p->getPlayerName(), p, this));
                Ogre::LogManager::getSingleton().logMessage("Using Kinect Controller");
            }
#endif
            if(!player) {
                player.reset(new KeyboardPlayer(p->getPlayerName(), p, this));
                Ogre::LogManager::getSingleton().logMessage("Using Keyboard Controller");
            }

        } break;

        case CONTROLLER_AI: {
            player.reset(new AiPlayer(g_settings.aiStrength, p->getPlayerName(), p, this));
        } break;

        case CONTROLLER_KEYBOARD: {
            player.reset(new KeyboardPlayer(p->getPlayerName(), p, this));
        } break;
    }
    _players.push_back(player);

    return player;
}

void PlayState::resetBall(BallPtr b) {
    if(_balls.size() != 1) {
        _balls.erase(std::find(_balls.begin(), _balls.end(), b));
    } else {
        b->setPosition(Ogre::Vector3(0, 0, 0));
        b->setSpeed(getRandomSpeed());
        b->setAccel(getRandomAccel());
    }
}

Ogre::Vector3 PlayState::getRandomSpeed() const {
    return Ogre::Vector3(Ogre::Math::RangeRandom(50, 100) * (Ogre::Math::UnitRandom() > 0.5 ? -1 : 1),
                         Ogre::Math::RangeRandom(50, 100) * (Ogre::Math::UnitRandom() > 0.5 ? -1 : 1),
                         Ogre::Math::RangeRandom(50, 100) * (Ogre::Math::UnitRandom() > 0.5 ? -1 : 1));
}

Ogre::Vector3 PlayState::getRandomAccel() const {
    return Ogre::Vector3(Ogre::Math::RangeRandom(10,20),
                         Ogre::Math::RangeRandom(10,20),
                         Ogre::Math::RangeRandom(10,20));
}

bool PlayState::frameRenderingQueued(const Ogre::FrameEvent& evt) {
    if(!GuiState::frameRenderingQueued(evt))
        return false;

    if(!_paused && !_ended) {
        bool ready = true;
        for(PlayerPtr p : _players) {
            p->update(evt.timeSinceLastFrame);
            ready &= p->isReady();
        }

        if(ready) {
            for(BallPtr b : _balls) {
                b->update(evt.timeSinceLastFrame);
            }

            for(PaddlePtr paddle : _paddles) {
                paddle->update(evt.timeSinceLastFrame);
            }

            checkHit();

            for(PlayerPtr player : _players) {
                if(!player->isAlive()) {
                    _ended = true;
                    _looser = player;
                    showEndScreen();
                }
            }
        }

        updateScore();
    } else if(_ended) {
        _endScreenTimeout -= evt.timeSinceLastFrame;
        if(_endScreenTimeout <= 0) {
            GameState* next = findByName("Pong 3D");
            if(next && next != this && _looser->isAI())
                changeGameState(next);
            else
                popGameState();
        }
    }

    return true;
}


void PlayState::checkHit() {
    OgreOggSound::OgreOggSoundManager* soundManager = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
    for(PaddlePtr p : _paddles) {
        const Ogre::AxisAlignedBox& bbox = p->getBoundingBox();
        bool isLeft = p->getPosition().x < 0;
        Ogre::Real collision = 0;
        if(isLeft)
            collision = bbox.getCorner(Ogre::AxisAlignedBox::NEAR_RIGHT_TOP).x;
        else
            collision = bbox.getCorner(Ogre::AxisAlignedBox::FAR_LEFT_BOTTOM).x;

        for(BallPtr b : _balls) {
            if(bbox.intersects(b->getBoundingBox())) {
                soundManager->getSound("pong")->play();
                b->reverse(Ball::DIR_X, collision);
                b->accelerate(b->getAccel());
            }
        }
    }

    for(BallPtr b : _balls) {
        if(!b)
            continue;
        auto pos = b->getPosition();

        if(pos.x > MAP_BBOX_X) {
            for(PlayerPtr p : _players) {
                if(p->getSide() == Player::SIDE_RIGHT) {
                    p->hit();
                    soundManager->getSound("plus")->play();
                }
            }
            resetBall(b);
        } else if(pos.x < -MAP_BBOX_X) {
            for(PlayerPtr p : _players) {
                if(p->getSide() == Player::SIDE_LEFT) {
                    p->hit();
                    soundManager->getSound("minus")->play();
                }
            }
            resetBall(b);
        } else if(fabs(pos.y) > MAP_BBOX_Y) {
            b->reverse(Ball::DIR_Y, pos.y < 0 ? -MAP_BBOX_Y : MAP_BBOX_Y);
        } else if(fabs(pos.z) > MAP_BBOX_Z) {
            b->reverse(Ball::DIR_Z, pos.z < 0 ? -MAP_BBOX_Z : MAP_BBOX_Z);
        }
    }
}

void PlayState::showEndScreen() {
    if(!_looser)
        return;

    _endScreenTimeout = 3.0; // show endscreen for 3 seconds

    using namespace CEGUI;

    WindowManager& wnmgr = WindowManager::getSingleton();

    Window* wnd = wnmgr.createWindow("TaharezLook/FrameWindow", "End Screen");
    wnd->setSize(USize(UDim(0.4, 0),
                       UDim(0.4, 0)));

    wnd->setPosition(UVector2(UDim(0.3, 0),
                              UDim(0.3, 0)));

    Window* label = wnmgr.createWindow("TaharezLook/Label", "Win Label");
    label->setSize(USize(UDim(1.0, 0.0), UDim(1.0, 0.0)));
    label->setProperty("HorzFormatting", "CentreAligned");
    label->setProperty("VertFormatting", "CentreAligned");

    OgreOggSound::OgreOggSoundManager* soundManager = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
    if(!_looser->isAI()) {
        label->setProperty("NormalTextColour", "FFFF0000");
        label->setText("Defeated");
        soundManager->getSound("lose")->play();
    } else {
        label->setProperty("NormalTextColour", "FF00FF00");
        label->setText("Victory");
        soundManager->getSound("win")->play();
    }
    wnd->addChild(label);

    System::getSingleton().getDefaultGUIContext().getRootWindow()->addChild(wnd);
}
