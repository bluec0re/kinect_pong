#include "paddle.h"

Paddle::Paddle(int color, const Ogre::String& player) : Object(color),
    _player(player)
{
    _mat = Ogre::MaterialManager::getSingleton().create(
                player, // name
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::Real red = ((color >> (8 << 1)) & 0xFF) / 255.0;
    Ogre::Real green = ((color >> (8 << 0)) & 0xFF) / 255.0;
    Ogre::Real blue = ((color >> (0)) & 0xFF) / 255.0;

    _mat->setReceiveShadows(false);
    _mat->setLightingEnabled(true);
    _mat->setAmbient(0, 0, 0);
    _mat->setDiffuse(red, green, blue, 0);
    _mat->setSelfIllumination(red,green,blue);

    Ogre::StringStream ss;
    ss << "r: " << red << " g: " << green << " b: " << blue << " t: " << _mat->getNumTechniques() << " p: " << _mat->getTechnique(0)->getNumPasses() << " a: " << _mat->getTechnique(0)->getPass(0)->getAmbient();
    Ogre::LogManager::getSingleton().logMessage(ss.str());
}

Paddle::~Paddle() {

}


void Paddle::create(Ogre::SceneManager* sceneMgr) {
    _entity = sceneMgr->createEntity(Ogre::SceneManager::PT_CUBE);
    _node = sceneMgr->getRootSceneNode()->createChildSceneNode();
    _node->attachObject(_entity);
    _node->setScale( 0.2, 0.5, 0.1);
    _entity->setMaterial(_mat);
}

void Paddle::setPosition(const Ogre::Vector3& pos) {
    if(abs(pos.x) > 190 || abs(pos.x) < 190)
        return;
    if(abs(pos.y) > 190)
        return;
    if(abs(pos.z) > 190)
        return;
    Object::setPosition(pos);
}
