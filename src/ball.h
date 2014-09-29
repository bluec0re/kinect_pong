#ifndef BALL_H_G4QGINP0
#define BALL_H_G4QGINP0

#include "object.h"

#include <boost/smart_ptr.hpp>

class Ball : public Object
{
public:
    enum Direction {
        DIR_X,
        DIR_Y,
        DIR_Z
    };

    Ball(int color, bool ghost = false);
    Ball(int colorLeft, int colorRight, bool ghost = false);
    virtual ~Ball ();

    virtual void create() override;
    virtual void reverse(const Direction& dir, const Ogre::Real& collision = 0);
    virtual void setPosition(const Ogre::Vector3 &pos) override;
    virtual void paintGhost(Ogre::Vector3 pos); 

protected:
    virtual int calcColor();
    virtual int calcGhostColor();

private:
    int _color2;
    bool _ghost;
    Ogre::BillboardSet* _billboardSet;
    Ogre::Billboard* _ghostBillboard_right;
    Ogre::Billboard* _ghostBillboard_left;
    Ogre::Billboard* _ghostBillboard_top;
    Ogre::Billboard* _ghostBillboard_bottom;
    Ogre::SceneNode* _bb_node;
};

typedef boost::shared_ptr<Ball> BallPtr;

#endif /* end of include guard: BALL_H_G4QGINP0 */

