#include "ball.h"

Ball::Ball(int color) :
    Object(color)
{
    _mat = Ogre::MaterialManager::getSingleton().create(
                "Ball", // name
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::Real red = ((color >> (8 << 1)) & 0xFF) / 255.0;
    Ogre::Real green = ((color >> (8 << 0)) & 0xFF) / 255.0;
    Ogre::Real blue = ((color >> (0)) & 0xFF) / 255.0;

    _mat->setReceiveShadows(false);
    _mat->setLightingEnabled(true);
    _mat->setAmbient(0, 0, 0);
    _mat->setDiffuse(red, green, blue, 0);
    _mat->setSelfIllumination(red,green,blue);

    //_mat->getTechnique(0)->getPass(0)->setEmissive(Ogre::ColourValue(red, green, blue, 1));

    Ogre::StringStream ss;
    ss << "r: " << red << " g: " << green << " b: " << blue << " t: " << _mat->getNumTechniques() << " p: " << _mat->getTechnique(0)->getNumPasses() << " a: " << _mat->getTechnique(0)->getPass(0)->getAmbient();
    Ogre::LogManager::getSingleton().logMessage(ss.str());
}

Ball::~Ball() {

}


void Ball::create(Ogre::SceneManager* sceneMgr) {
    _entity = sceneMgr->createEntity(Ogre::SceneManager::PT_SPHERE);
    _node = sceneMgr->getRootSceneNode()->createChildSceneNode();
    _node->attachObject(_entity);
    _node->setScale( 0.1, 0.1, 0.1);
    _entity->setMaterial(_mat);
}

void Ball::setPosition(const Ogre::Vector3& pos) {
    if(abs(pos.x) > 190) {
        _speed.x *= -1;
    } else if(abs(pos.y) > 190) {
        _speed.y *= -1;
    } else if(abs(pos.z) > 190) {
        _speed.z *= -1;
    } else
        Object::setPosition(pos);
}
