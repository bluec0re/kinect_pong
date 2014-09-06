#include "global.h"
#include "paddle.h"

Paddle::Paddle(int color, const Ogre::String& player, bool transparent) : Object(color),
    _player(player), _transparent(transparent)
{
    _mat = Ogre::MaterialManager::getSingleton().create(
                player, // name
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::Real red = ((color >> (8 << 1)) & 0xFF) / 255.0;
    Ogre::Real green = ((color >> (8 << 0)) & 0xFF) / 255.0;
    Ogre::Real blue = ((color >> (0)) & 0xFF) / 255.0;

    _mat->setReceiveShadows(false);
    _mat->setLightingEnabled(true);
    _mat->setAmbient(red, green, blue);
    _mat->setDiffuse(red, green, blue, 0.5);
    //_mat->setSelfIllumination(red,green,blue);
    if(transparent) {
        _mat->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
        _mat->setDepthWriteEnabled(false);
        Ogre::TextureUnitState* unit = new Ogre::TextureUnitState(_mat->getTechnique(0)->getPass(0));
        unit->setColourOperationEx(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, Ogre::ColourValue(red, green, blue));
        unit->setAlphaOperation(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, 0.5);
        _mat->getTechnique(0)->getPass(0)->addTextureUnitState(unit);
    }

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
    _node->setScale( 0.2, 0.5, 0.5);
    _entity->setMaterial(_mat);
}

void Paddle::setPosition(const Ogre::Vector3& tmp) {
    Ogre::Vector3 pos(tmp);
    if(abs(pos.x) > MAP_BBOX_X || abs(pos.x) < MAP_BBOX_X)
        return;
    if(abs(pos.y) > MAP_BBOX_Y)
        pos.y = MAP_BBOX_Y * (static_cast<int>(pos.y)/abs(pos.y));
    if(abs(pos.z) > MAP_BBOX_Z)
        pos.z = MAP_BBOX_Z * (static_cast<int>(pos.z)/abs(pos.z));
    Object::setPosition(pos);
}
