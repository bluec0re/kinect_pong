#include "global.h"
#include "playstate.h"
#include "gamestatemanager.h"

#include "keyboardplayer.h"
#include "aiplayer.h"

#ifdef HAVE_OPENNI2
#include "kinect.h"
#include "kinectplayer.h"
#endif

#define MAP_SIZE 200
// TODO: create map from following vars
#define MAP_X 400
#define MAP_Y 200
#define MAP_Z 200
#define MAP_BBOX 190
// TODO: create map from following vars
#define MAP_BBOX_X 390
#define MAP_BBOX_Y 190
#define MAP_BBOX_Z 190


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
    CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    /*CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");
    CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);*/

    Ogre::Light* pointLight = mDevice->sceneMgr->createLight("pointLight");
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setPosition(Ogre::Vector3::ZERO);
    pointLight->setDiffuseColour(1.0, 1.0, 1.0);
    pointLight->setSpecularColour(1.0, 1.0, 1.0);

    {
        PaddlePtr p = addPaddle(0xFF0000, "Player 1", -MAP_BBOX_X);
        PlayerPtr player;
#ifdef HAVE_OPENNI2
        if(Kinect::getInstance()->isConnected()) {
            // preview
            CEGUI::Texture& cetex = mDevice->guiRenderer->createTexture("depthMap", Kinect::getInstance()->getDepthImage());
            const CEGUI::Rectf rect(CEGUI::Vector2f(0.0f, 0.0f), cetex.getOriginalDataSize());
            CEGUI::BasicImage* image = (CEGUI::BasicImage*)( &CEGUI::ImageManager::getSingleton().create("BasicImage", "RTTImage"));
               image->setTexture(&cetex);
               image->setArea(rect);
               image->setAutoScaled(CEGUI::ASM_Both);

            CEGUI::Window *si = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage", "RTTWindow");
            si->setSize(CEGUI::USize(CEGUI::UDim(0.5f, 0),
                                     CEGUI::UDim(0.4f, 0)));
            si->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f, 0),
                                            CEGUI::UDim(0.0f, 0)));
            si->setProperty("Image", "RTTImage");
            //sheet->addChild(si);
            CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(si);

            player.reset(new KinectPlayer("Player 1", p.get(), this));
        }
#endif
        if(!player)
            player.reset(new KeyboardPlayer("Player 1", p.get(), this));
        _players.push_back(player);
    }

    {        
        PaddlePtr p = addPaddle(0x0000FF, "Player 2", MAP_BBOX_X);
        PlayerPtr player(new AiPlayer(AiPlayer::AI_WEAK, "Player 2", p.get(), this));
        _players.push_back(player);
    }



    BallPtr ball = addBall();

    resetBall(ball);
}

void PlayState::exit() {
    _paddles.clear();
    _balls.clear();
    _players.clear();

    mDevice->sceneMgr->destroyAllManualObjects();
    mDevice->sceneMgr->getRootSceneNode()->removeAndDestroyAllChildren();
    mDevice->sceneMgr->destroyAllLights();
    /*mDevice->sceneMgr->destroySceneNode("box_node");
    mDevice->sceneMgr->destroySceneNode("box_subnode");*/
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
    }
    return true;
}

BallPtr PlayState::addBall() {
    BallPtr ball(new Ball(0x00FF00));
    ball->create(mDevice->sceneMgr);
    ball->setAccel(getRandomAccel());
    ball->enableAccel(false);
    ball->setSpeed(getRandomSpeed());
    _balls.push_back(ball);

    return ball;
}

PaddlePtr PlayState::addPaddle(int color, const Ogre::String& name, const Ogre::Real& pos) {
    PaddlePtr p(new Paddle(color, name));
    p->create(mDevice->sceneMgr);
    p->setPosition(Ogre::Vector3(pos, 0, 0));
    _paddles.push_back(p);

    return p;
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
    return Ogre::Vector3(Ogre::Math::RangeRandom(25, 75) * (Ogre::Math::UnitRandom() > 0.5 ? -1 : 1));
}

Ogre::Vector3 PlayState::getRandomAccel() const {
    return Ogre::Vector3(Ogre::Math::RangeRandom(10,20));
}

bool PlayState::frameRenderingQueued(const Ogre::FrameEvent& evt) {
    if(!GuiState::frameRenderingQueued(evt))
        return false;

    checkHit();

    for(BallPtr b : _balls) {
        b->update(evt.timeSinceLastFrame);
    }

    for(PaddlePtr paddle : _paddles) {
        paddle->update(evt.timeSinceLastFrame);
    }

    for(PlayerPtr p : _players) {
        p->update(evt.timeSinceLastFrame);
    }

    return true;
}


void PlayState::checkHit() {
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
                b->reverse(Ball::DIR_X, collision);
                b->accelerate(b->getAccel());
            }
        }
    }

    for(BallPtr b : _balls) {
        auto pos = b->getPosition();

        if(pos.x > MAP_BBOX_X) {
            for(PlayerPtr p : _players) {
                if(p->getSide() == Player::SIDE_RIGHT)
                    p->hit();
            }
            resetBall(b);
        } else if(pos.x < -MAP_BBOX_X) {
            for(PlayerPtr p : _players) {
                if(p->getSide() == Player::SIDE_LEFT)
                    p->hit();
            }
            resetBall(b);
        } else if(abs(pos.y) > MAP_BBOX_Y) {
            b->reverse(Ball::DIR_Y, pos.y < 0 ? -MAP_BBOX_Y : MAP_BBOX_Y);
        } else if(abs(pos.z) > MAP_BBOX_Z) {
            b->reverse(Ball::DIR_Z, pos.z < 0 ? -MAP_BBOX_Z : MAP_BBOX_Z);
        }
    }
}
