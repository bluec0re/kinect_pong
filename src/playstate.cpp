#include "playstate.h"
#include "gamestatemanager.h"

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
    myManualObject->position(-200, -200, -200); // 0
    myManualObject->position(-200, -200,  200);
    myManualObject->position(-200,  200,  200);
    myManualObject->position(-200,  200, -200); // 3
    myManualObject->position( 200, -200, -200);
    myManualObject->position( 200, -200,  200); // 5
    myManualObject->position( 200,  200,  200);
    myManualObject->position( 200,  200, -200); // 7

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
    int step = 400/(loops+1);

    // xy
    for(int l = 1; l <= loops; l++) {
        for(int a = -1; a < 2; a+=2) {
            for(int b = -1; b < 2; b+=2) {
                Ogre::Vector3 pos(200*a, 200*b*a, -200 + l*step);
                myManualObject->position(pos);
                positions.push_back(pos);
            }
        }
    }

    // xz
    for(int l = 1; l <= loops; l++) {
        for(int a = -1; a < 2; a+=2) {
            for(int b = -1; b < 2; b+=2) {
                Ogre::Vector3 pos(200*b*a, -200 + l*step, 200*a);
                myManualObject->position(pos);
                positions.push_back(pos);
            }
        }
    }

    // yz
    for(int l = 1; l <= loops; l++) {
        for(int a = -1; a < 2; a+=2) {
            for(int b = -1; b < 2; b+=2) {
                Ogre::Vector3 pos(-200 + l*step, 200*b*a, 200*a);
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
        boost::shared_ptr<Paddle> p(new Paddle(0xFF0000, "Player 1"));
        p->create(mDevice->sceneMgr);
        p->setPosition(Ogre::Vector3(-190, 0, 0));
        _paddles.push_back(p);
    }

    {
        boost::shared_ptr<Paddle> p(new Paddle(0x0000FF, "Player 2"));
        p->create(mDevice->sceneMgr);
        p->setPosition(Ogre::Vector3(190, 0, 0));
        _paddles.push_back(p);
    }

    _ball.reset(new Ball(0x00FF00));
    _ball->create(mDevice->sceneMgr);
    _ball->setSpeed(Ogre::Vector3(50,75,0));
}

void PlayState::exit() {
    _paddles.clear();
    _ball.reset();

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

bool PlayState::frameRenderingQueued(const Ogre::FrameEvent& evt) {
    _ball->update(evt.timeSinceLastFrame);

    for(boost::shared_ptr<Paddle> paddle : _paddles) {
        paddle->update(evt.timeSinceLastFrame);
    }

    return true;
}
