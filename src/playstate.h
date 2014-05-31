#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include "gamestate.h"
#include "paddle.h"
#include "ball.h"

class PlayState : public GameState
{
public:
    PlayState();
    /**
     * create the box around the game field
     *
     * @brief createBox
     * @param red
     * @param green
     * @param blue
     * @param clipPlane  lines behind this plane would not be drawn
     */
    virtual void createBox(Ogre::Real red, Ogre::Real green, Ogre::Real blue, const Ogre::Plane& clipPlane);

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
    virtual void createSubBox(Ogre::Real red, Ogre::Real green, Ogre::Real blue, const Ogre::Plane& clipPlane, Ogre::SceneNode* parent);

    virtual void enter();
    virtual void exit();
    virtual bool pause() { return true; }
    virtual void resume() {}

    virtual bool keyPressed(const OIS::KeyEvent &arg) { return true; }
    virtual bool keyReleased(const OIS::KeyEvent &arg);
    virtual bool mouseMoved( const OIS::MouseEvent &arg ) { return true; }
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) { return true; }
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) { return true; }

    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

protected:
    std::vector<boost::shared_ptr<Paddle> > _paddles; //
    boost::shared_ptr<Ball> _ball;
};

#endif // PLAYSTATE_H
