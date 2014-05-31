#ifndef __gamestate_h__
#define __gamestate_h__
 
#include "global.h"
 
class GameState;
 
/** \class GameStateListener
    Allows a GameStateManager to recieve callbacks
    from a GameState. */
class GameStateListener
{
public:
    /** Constructor */
    GameStateListener(void) {};
 
    /** Virtual Deconstructor */
    virtual ~GameStateListener(void) {};
 
    /** Store a game state to manage. */
    virtual void ManageGameState(Ogre::String state_name,GameState*state)=0;
 
    /** Find a state by name. */
    virtual GameState *findByName(Ogre::String state_name)=0;
 
    /** Request a change to state. */
    virtual void changeGameState(GameState *state)=0;
 
    /** Push state onto the stack. */
    virtual bool pushGameState(GameState* state)=0;
 
    /** Pop a game state off the stack. */
    virtual void popGameState()=0;
 
    /** Cause a shutdown. */
    virtual void Shutdown()=0;
 
};
 
/** \class GameState
    Inherit this class to make a game state capable
    of being mananged by the game state manager. 
    Be sure to use DECLARE_GAMESTATE_CLASS(class)
    in your public section.
*/
class GameState: public Ogre::FrameListener,
    public OIS::KeyListener,
    public OIS::MouseListener
{
public:
    /** Do not inherit this directly! Use DECLARE_GAMESTATE_CLASS (class) to do it for you. */
    static GameState* Create(GameStateListener *parent, const Ogre::String& name) { return 0; };
 
    /** Destroy self. */
    void destroy(void)
    { delete this; }
 
    /** Initialize the game state with device information. */
    void init(device_info *devices)
    { mDevice=devices; }
 
    /** Inherit to supply game state enter code. */
    virtual void enter(void)=0;
    /** Inherit to supply state exit code. */
    virtual void exit(void)=0;
    /** Inherit to supply pause code. Inherit only if this game state can be paused.
        Return true for successful pause, or false to deny pause. */
    virtual bool pause(void)
    { return false; }
    /** Inherit to supply resume code. Inherit only if this game state can be paused. */
    virtual void resume(void) {};
 
protected:
    /** Constructor: This should be a private member of an inherited class. */
    GameState(void) {};
    /** Destructor: This should be a private member of an inherited class. */
    virtual ~GameState(void) {};
 
    /** Find a state by its name. */
    GameState *findByName(Ogre::String state_name)
    { return parent->findByName(state_name); }
 
    /** Request a change to game state. */
    void changeGameState(GameState *state) 
    { parent->changeGameState(state); }
 
    /** Push game state onto the stack. */
    bool pushGameState(GameState* state) 
    { return parent->pushGameState(state); }
 
    /** Pop a game state off the stack. */
    void popGameState(void) 
    { parent->popGameState(); }
 
    /** Cause a shutdown. */
    void Shutdown(void)
    { parent->Shutdown(); }
 
    /** Stores the GameStateManager which is managing this state. */
    GameStateListener *parent;
    /** Keeps a method of device interaction. */
    device_info *mDevice;
};
 
/** Create the game state. Inherit, Create your class, and have it managed.
    Example:
    \code
    static void MyGameStateClass::Create(GameStateListener *parent,
            const Ogre::String name)
    {
        myGameStateClass myGameState=new myGameStateClass(); 
        myGameState->parent=parent;
        parent->ManageGameState(name,myGameState);
    }
    \endcode
*/
#define DECLARE_GAMESTATE_CLASS(T) static GameState* Create(GameStateListener *parent, const Ogre::String& name)  { \
        T *myGameState=new T();\
        myGameState->parent=parent;\
        parent->ManageGameState(name,myGameState);\
        return myGameState; \
    }
 
#endif
