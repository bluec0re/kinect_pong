#include "ball.h"
#include "global.h"

#define BYTE(in, pos) (static_cast<unsigned char>((in >> pos*8) & 0xFF))
#define INTERPOLATE(from, to, pos, perc) ((static_cast<unsigned char>((BYTE(to, pos) - BYTE(from, pos)) * perc) \
                                                                + BYTE(from, pos)) << pos * 8)
#define CORRECT_SIGN(attr) ((collision < 0 ? bbox.getHalfSize().attr : -bbox.getHalfSize().attr) * (fabs(pos.attr) < fabs(collision) ? 2 : 1))

Ball::Ball(int color, bool ghost) :
    Object(color), _color2(color), _ghost(ghost) {}

Ball::Ball(int colorLeft, int colorRight, bool ghost) :
        Object(colorLeft), _color2(colorRight), _ghost(ghost), _billboardSet(nullptr)
{
    _mat = Ogre::MaterialManager::getSingleton().create(
                "Ball", // name
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    _mat->setReceiveShadows(false);
    _mat->setLightingEnabled(true);
}

Ball::~Ball() {
    _billboardSet->removeBillboard(_ghostBillboard_right);
    _billboardSet->removeBillboard(_ghostBillboard_left);
    _billboardSet->removeBillboard(_ghostBillboard_top);
    _billboardSet->removeBillboard(_ghostBillboard_bottom);
}

int Ball::calcColor() {
    Ogre::Real pos = getPosition().x + MAP_X;
    Ogre::Real percentage = pos / (MAP_X*2);

    return INTERPOLATE(_color, _color2, 0, percentage) |
            INTERPOLATE(_color, _color2, 1, percentage) |
            INTERPOLATE(_color, _color2, 2, percentage) |
            INTERPOLATE(_color, _color2, 3, percentage);
}

int Ball::calcGhostColor() {
    Ogre::Real pos = getPosition().x + MAP_X;
    Ogre::Real percentage = pos / (MAP_X*2);

    return INTERPOLATE(0xFF0000FF, 0xFFFF0000, 0, percentage) |
            INTERPOLATE(0xFF0000FF, 0xFFFF0000, 1, percentage) |
            INTERPOLATE(0xFF0000FF, 0xFFFF0000, 2, percentage) |
            INTERPOLATE(0xFF0000FF, 0xFFFF0000, 3, percentage);
}

void Ball::create() {
    _entity = _sceneMgr->createEntity(Ogre::SceneManager::PT_SPHERE);
    _node = _sceneMgr->getRootSceneNode()->createChildSceneNode();
    _node->attachObject(_entity);
    _node->setScale( 0.1, 0.1, 0.1);
    //_node->showBoundingBox(true);
    _entity->setMaterial(_mat);

    _bb_node = _sceneMgr->getRootSceneNode()->createChildSceneNode();
    if(_billboardSet == nullptr) {
        _billboardSet = _sceneMgr->createBillboardSet();
        _billboardSet->setMaterialName("Flare");
    }
    if(_ghost == true) {
        _ghostBillboard_right = _billboardSet->createBillboard(Ogre::Vector3::ZERO);
        _ghostBillboard_right->setColour(Ogre::ColourValue::Blue);
        _ghostBillboard_top = _billboardSet->createBillboard(Ogre::Vector3::ZERO);
        _ghostBillboard_top->setColour(Ogre::ColourValue::Blue);
        _ghostBillboard_left = _billboardSet->createBillboard(Ogre::Vector3::ZERO);
        _ghostBillboard_left->setColour(Ogre::ColourValue::Blue);
        _ghostBillboard_bottom = _billboardSet->createBillboard(Ogre::Vector3::ZERO);
        _ghostBillboard_bottom->setColour(Ogre::ColourValue::Blue);
    }
    _bb_node->attachObject(_billboardSet);
}

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
            pos.x = collision + CORRECT_SIGN(x)*1.5f;
        break;

    case Ball::DIR_Y:
        ss << "Y ";
        _speed.y *= -1;
        if(collision != 0)
            pos.y = collision + CORRECT_SIGN(y)*1.5f;
        break;

    case Ball::DIR_Z:
        ss << "Z ";
        _speed.z *= -1;
        if(collision != 0)
            pos.z = collision + CORRECT_SIGN(z)*1.5f;
        break;
    }
    setPosition(pos);
    ss << "New Pos " << pos << ". ";

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_TRIVIAL, ss.str());
}

void Ball::setPosition(const Ogre::Vector3 &pos) {
    int color = calcColor();
    Ogre::Real red = BYTE(color, 2) / 255.0f;
    Ogre::Real green = BYTE(color, 1) / 255.0f;
    Ogre::Real blue = BYTE(color, 0) / 255.0f;
    _mat->setAmbient(red, green, blue);
    _mat->setDiffuse(red, green, blue, 1.0);


    Ogre::StringStream ss;
    ss << "r: " << red << " g: " << green << " b: " << blue << " t: " << _mat->getNumTechniques() << " p: " << _mat->getTechnique(0)->getNumPasses() << " a: " << _mat->getTechnique(0)->getPass(0)->getAmbient();
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_TRIVIAL, ss.str());

    Object::setPosition(pos);
    if(_ghost == true) {
        paintGhost(pos);
    }
}

void Ball::paintGhost(Ogre::Vector3 pos) {
    Ogre::ColourValue color = Ogre::ColourValue(calcGhostColor());
    std::cout << std::hex << calcGhostColor() << std::endl;
    _ghostBillboard_right->setColour(color);
    _ghostBillboard_left->setColour(color);
    _ghostBillboard_top->setColour(color);
    _ghostBillboard_bottom->setColour(color);

    Ogre::Vector3 tmp = pos;
    //pos.y = 0;
    pos.z = MAP_Z;
    _ghostBillboard_right->setPosition(pos);
    pos.z -= (2 * MAP_Z);
    _ghostBillboard_left->setPosition(pos);
    //tmp.z = 0;
    tmp.y = MAP_Y;
    _ghostBillboard_top->setPosition(tmp);
    tmp.y -= (2 * MAP_Y);
    _ghostBillboard_bottom->setPosition(tmp);
}
