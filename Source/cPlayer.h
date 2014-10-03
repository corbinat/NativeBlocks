// A player is one person's playing surface. This is a 6x12 grid and contains
// children objects, such as the beans and floor

#ifndef ___cPlayer_h___
#define ___cPlayer_h___

#include "cObject.h"
#include "cMessageDispatcher.h"
#include "cStaging.h"

#include <unordered_set>

// Size is 12 + 5 for garbage queue
static const uint32_t g_kTotalRows = 17;

enum ePlayerState
{
   kStateIdle,
   kStateCreateBeans,
   kStateControlBeans,
   kStateWaitForBeansToSettle,
   kStateCheckForMatches,
   kStateDropGarbage,
   kStateCheckForLosingState
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
class cBeanInfo;

class cPlayer: public cObject
{
public:

   cPlayer(cResources* a_pResources, std::minstd_rand a_RandomNumberEngine, std::string a_Identifier);
   virtual ~cPlayer();

   // These functions are overloaded from cObject
   virtual void Event(std::list<sf::Event> * a_pEventList);
   virtual void Step (uint32_t a_ElapsedMiliSec);
   virtual void Collision(cObject* a_pOther);

   // These functions are derived by the "Brains"
   virtual void StateChange(ePlayerState a_Old, ePlayerState a_New) = 0;
   virtual void ControlBeans(uint32_t a_ElapsedMiliSec) = 0;

   void MessageReceived(sMessage a_Message);

   sf::Vector2<cBean*> GetBeansInPlay();
   sf::Vector2<uint32_t> GetBeanGridPosition(cBean* a_pBean);

   // Function used to move beans during the kStateControlBeans state. This is
   // called due to user input or to move beans down. Returns true if success.
   bool MoveControlledBeans(sf::Vector3<double> a_NewRelativePosition);

   // This function moves the beans by one grid size left or right.
   bool ShiftControlledBeansColumn(int32_t a_RelativeColumn);

   void RotateBeans(eRotationDirection a_Rotation);
   eRotationState GetRotationState();

   void SetFastFall(bool a_FastFall);

   ePlayerState GetPlayerState();


   std::vector<std::vector<std::shared_ptr<cBeanInfo>>> ClonePlayingField();

   // TODO: This doesn't need to be a member function
   uint32_t CalculateScore(
      uint32_t a_Matches,
      uint32_t a_Multiplier,
      uint32_t a_Groups,
      uint32_t a_Chains
      );

private:

   // Create initial playing surface for this object
   void _Initialize();

   // Begin dropping beans
   void _StartGame();

   bool _BeansAreResting();

   void _CreateGarbageBean(uint32_t a_Column, uint32_t a_Row);

   uint32_t _CalculateGarbageBeanNumber();

   bool m_Initialized;
   ePlayerState m_CurrentState;
   eRotationState m_RotationState;

   // Beans in play
   cBean* m_pPivotBean;
   cBean* m_pSwingBean;

   cStaging m_Staging;

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

   // We must wait in the settle time state for a minimum amount of time
   int32_t m_TotalSettleTime;
   const uint32_t m_MinSettleTime;

   // Scoring--------
   // This counts how many beans exploded. It resets once beans fall.
   uint32_t m_BeansExploded;

   // This holds the multiplier, which goes up for every extra bean exploded. It
   // resets once beans fall.
   uint32_t m_ScoreMultiplier;

   // This holds how many separate groups exploded. It resets once beans fall.
   uint32_t m_NumberDifferentGroups;

   // This holds how many chain reactions have gone off. It resets once new
   // beans are created.
   uint32_t m_ChainCount;

   // This holds how many garbage beans need to fall on this player.
   uint32_t m_GarbageAcumulator;

   // Prevents multple garbage bean groups from falling in one round
   bool m_GarbageDropped;

};

#endif
