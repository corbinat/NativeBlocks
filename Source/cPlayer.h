// A player is one person's playing surface. This is a 6x12 grid and contains
// children objects, such as the beans and floor

#ifndef ___cPlayer_h___
#define ___cPlayer_h___

#include "cObject.h"
#include "cMessageDispatcher.h"

enum ePlayerState
{
   kStateIdle,
   kStateCreateBeans,
   kStateControlBeans,
   kStateWaitForBeansToSettle,
   kStateCheckForMatches
};

// Spinning bean relative to the pivot bean. Starts Up.
enum eRotationState
{
   kRotationStateUp,
   kRotationStateDown,
   kRotationStateLeft,
   kRotationStateRight
};

enum eRotationDirection
{
   kRotateClockwise,
   kRotateCounterClockwise
};

class cBean;

class cPlayer: public cObject
{
public:

   cPlayer(cResources* a_pResources);
   virtual ~cPlayer();

   // These functions are overloaded from cObject
   virtual void Event(std::list<sf::Event> * a_pEventList);
   virtual void Step (uint32_t a_ElapsedMiliSec);
   virtual void Collision(cObject* a_pOther);

   void MessageReceived(sMessage a_Message);

private:

   // Create initial playing surface for this object
   void _Initialize();

   // Begin dropping beans
   void _StartGame();

   bool _BeansAreResting();

   // Function used to move beans during the kStateControlBeans state. This is
   // called due to user input or to move beans down. Returns true if success.
   bool _MoveControlledBeans(sf::Vector3<double> a_NewRelativePosition);

   void _RotateBeans(eRotationDirection a_Rotation);

   bool m_Initialized;
   ePlayerState m_CurrentState;
   eRotationState m_RotationState;

   // Beans in play
   cBean* m_pPivotBean;
   cBean* m_pSwingBean;

   // These are beans we need to wait on to finish falling
   std::list<cBean*> m_FallingBeans;

   // These are beans that just fell so that we can check them for matches.
   std::vector<cBean*> m_NewBeans;

   // These are beans that have settled on the playing field
   std::vector<std::vector<cBean*>> m_Beans;

   // How long to wait before moving the bean down one
   uint32_t m_MiliSecPerFall;
   uint32_t m_MiliSecSinceLastFall;

   // Count how long a bean has been resting. Lock bean in when this hits max
   uint32_t m_RestingBeanTimer;
   const uint32_t m_RestingLimit;
   bool m_BeanIsResting;

   // If the user is pressing Down then make the bean fall fast
   bool m_FastFall;

   bool m_BeanControlEnabled;

   uint32_t m_KeyRepeatTime;
   const uint32_t m_KeyRepeatLimit;
   bool m_LeftKeyDown;
   bool m_RightKeyDown;

   // We must wait in the settle time state for a minimum amount of time
   int32_t m_TotalSettleTime;
   const uint32_t m_MinSettleTime;

};

#endif
