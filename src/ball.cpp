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
    _mat->setAmbient(red, green, blue);
    _mat->setDiffuse(red, green, blue, 1.0);
    //_mat->setSelfIllumination(red,green,blue);

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
    //_node->showBoundingBox(true);
    _entity->setMaterial(_mat);
}

#define CORRECT_SIGN(attr) ((collision < 0 ? bbox.getHalfSize().attr : -bbox.getHalfSize().attr) * (fabs(pos.attr) < fabs(collision) ? 2 : 1))

void Ball::reverse(const Direction& dir, const Ogre::Real& collision) {
    Ogre::Vector3 pos = getPosition();
    Ogre::AxisAlignedBox bbox = _entity->getBoundingBox();
    bbox.scale(_node->getScale());
    Ogre::Real radius = _entity->getBoundingRadius() * _node->getScale().x;

    Ogre::StringStream ss;
    ss << "Ball speed reversed. ";
    ss << bbox.getHalfSize() << ". ";
    ss << radius << ". ";
    ss << "Collision " << collision << ". ";
    ss << "Old Pos " << pos << ". ";

    switch(dir) {
    case Ball::DIR_X:
        ss << "X ";
        _speed.x *= -1;
        if(collision != 0)
            pos.x = collision + CORRECT_SIGN(x);
        break;

    case Ball::DIR_Y:
        ss << "Y ";
        _speed.y *= -1;
        if(collision != 0)
            pos.y = collision + CORRECT_SIGN(y);
        break;

    case Ball::DIR_Z:
        ss << "Z ";
        _speed.z *= -1;
        if(collision != 0)
            pos.z = collision + CORRECT_SIGN(z);
        break;
    }
    setPosition(pos);
    ss << "New Pos " << pos << ". ";

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_TRIVIAL, ss.str());
}
