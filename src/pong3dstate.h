#ifndef PON32DSTATE_H
#define PON32DSTATE_H

#include "playstate.h"

class Pong3DState : public PlayState
{
public:
    Pong3DState();
    DECLARE_GAMESTATE_CLASS(Pong3DState)

    virtual void enter();
};

#endif // PON32DSTATE_H
