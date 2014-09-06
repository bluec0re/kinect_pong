#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include "guistate.h"
#include "paddle.h"
#include "ball.h"
#include "player.h"


class PlayState : public GuiState
{
public:
    using GuiState::GuiState;

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

    virtual bool keyReleased(const OIS::KeyEvent &arg);

    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
    virtual void checkHit();

    virtual void resetBall(BallPtr b);
    virtual BallPtr addBall();
    virtual PaddlePtr addPaddle(int color, const Ogre::String& name, const Ogre::Real& pos);
    virtual const std::vector<BallPtr>& getBalls() const { return _balls; }

protected:
    virtual Ogre::Vector3 getRandomSpeed() const;
    virtual Ogre::Vector3 getRandomAccel() const;

protected:
    std::vector<PaddlePtr> _paddles; //
    std::vector<BallPtr> _balls;
    std::vector<PlayerPtr> _players;
};

#endif // PLAYSTATE_H
