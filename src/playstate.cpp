#include "playstate.h"
#include "gamestatemanager.h"

#include "keyboardplayer.h"
#include "aiplayer.h"

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

PlayState::PlayState()
{
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
    myManualObject->position(-MAP_SIZE, -MAP_SIZE, -MAP_SIZE); // 0
    myManualObject->position(-MAP_SIZE, -MAP_SIZE,  MAP_SIZE);
    myManualObject->position(-MAP_SIZE,  MAP_SIZE,  MAP_SIZE);
    myManualObject->position(-MAP_SIZE,  MAP_SIZE, -MAP_SIZE); // 3
    myManualObject->position( MAP_SIZE, -MAP_SIZE, -MAP_SIZE);
    myManualObject->position( MAP_SIZE, -MAP_SIZE,  MAP_SIZE); // 5
    myManualObject->position( MAP_SIZE,  MAP_SIZE,  MAP_SIZE);
    myManualObject->position( MAP_SIZE,  MAP_SIZE, -MAP_SIZE); // 7

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
    myManualObjectMaterial->getTechnique(0)->getPass(0)->setAmbient(0,0,1);
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
                Ogre::Vector3 pos(MAP_SIZE*a, MAP_SIZE*b*a, -MAP_SIZE + l*step);
                myManualObject->position(pos);
                positions.push_back(pos);
            }
        }
    }

    // xz
    for(int l = 1; l <= loops; l++) {
        for(int a = -1; a < 2; a+=2) {
            for(int b = -1; b < 2; b+=2) {
                Ogre::Vector3 pos(MAP_SIZE*b*a, -MAP_SIZE + l*step, MAP_SIZE*a);
                myManualObject->position(pos);
                positions.push_back(pos);
            }
        }
    }

    // yz
    for(int l = 1; l <= loops; l++) {
        for(int a = -1; a < 2; a+=2) {
            for(int b = -1; b < 2; b+=2) {
                Ogre::Vector3 pos(-MAP_SIZE + l*step, MAP_SIZE*b*a, MAP_SIZE*a);
                myManualObject->position(pos);
                positions.push_back(pos);
            }
        }
    }

    // connect the vertices
    for(int l = 0; l < loops*3; l++) {
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
    {
        PaddlePtr p(new Paddle(0xFF0000, "Player 1"));
        p->create(mDevice->sceneMgr);
        p->setPosition(Ogre::Vector3(-MAP_BBOX, 0, 0));
        _paddles.push_back(p);

        PlayerPtr player(new KeyboardPlayer(p.get(), this));
        _players.push_back(player);
    }

    {
        PaddlePtr p(new Paddle(0x0000FF, "Player 2"));
        p->create(mDevice->sceneMgr);
        p->setPosition(Ogre::Vector3(MAP_BBOX, 0, 0));
        _paddles.push_back(p);

        PlayerPtr player(new AiPlayer(p.get(), this));
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
    /*mDevice->sceneMgr->destroySceneNode("box_node");
    mDevice->sceneMgr->destroySceneNode("box_subnode");*/
}

bool PlayState::keyReleased(const OIS::KeyEvent &arg) {
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
    Ogre::Vector3 speed;
    speed.x = getRandomSpeed();
    speed.y = getRandomSpeed();
    speed.z = getRandomSpeed();
    ball->setSpeed(speed);
    _balls.push_back(ball);

    return ball;
}

void PlayState::resetBall(BallPtr b) {
    if(_balls.size() != 1) {
        _balls.erase(std::find(_balls.begin(), _balls.end(), b));
    } else {
        b->setPosition(Ogre::Vector3(0, 0, 0));
        Ogre::Vector3 speed;
        speed.x = getRandomSpeed();
        speed.y = getRandomSpeed();
        speed.z = getRandomSpeed();
        b->setSpeed(speed);
        b->setAccel(getRandomAccel());
    }
}

Ogre::Real PlayState::getRandomSpeed() const {
    return Ogre::Math::RangeRandom(25, 75) * (Ogre::Math::UnitRandom() > 0.5 ? -1 : 1);
}

Ogre::Vector3 PlayState::getRandomAccel() const {
    return Ogre::Vector3(Ogre::Math::RangeRandom(10,20));
}

bool PlayState::frameRenderingQueued(const Ogre::FrameEvent& evt) {
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

        for(BallPtr b : _balls) {
            if(bbox.intersects(b->getBoundingBox())) {
                b->reverse(Ball::DIR_X);
            }
        }
    }

    for(BallPtr b : _balls) {
        if(b->getPosition().x > MAP_BBOX) {
            resetBall(b);
        } else if(b->getPosition().x < -MAP_BBOX) {
            // WIN OR LOOSE
            resetBall(b);
        } else if(b->getPosition().y > MAP_BBOX || b->getPosition().y < -MAP_BBOX) {
            b->reverse(Ball::DIR_Y);
        } else if(b->getPosition().z > MAP_BBOX || b->getPosition().z < -MAP_BBOX) {
            b->reverse(Ball::DIR_Z);
        }
    }
}
