// A player is one person's playing surface. This is a 6x12 grid and contains
// children objects, such as the beans and floor

#ifndef ___cPlayer_h___
#define ___cPlayer_h___

#include "cObject.h"
#include "cMessageDispatcher.h"

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
   kStateDropGarbage
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

   cPlayer(cResources* a_pResources);
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


   // These functions are used by the AI--------------------

   // This function returns the resulting score if a player were to play the
   // provided beans
   uint32_t SimulatePlay(
      std::shared_ptr<cBeanInfo> a_pBean1,
      std::shared_ptr<cBeanInfo> a_pBean2,
      std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField
      );

   std::vector<std::vector<std::shared_ptr<cBeanInfo>>> ClonePlayingField();

private:

   // Create initial playing surface for this object
   void _Initialize();

   // Begin dropping beans
   void _StartGame();

   bool _BeansAreResting();

   uint32_t _ConnectBeanToNeighbors(
      std::shared_ptr<cBeanInfo> a_pBean,
      std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField
      );

   uint32_t _ConnectColumnNeighbors(
      uint32_t a_Column,
      std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField
      );

   bool _BubbleBeansDown(std::vector<std::shared_ptr<cBeanInfo>>& a_Column);

   // returns how many exploded
   uint32_t _SearchColumnAndExplodeConnections(
      uint32_t a_Column,
      std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField,
      std::unordered_set<uint32_t>* a_pNewColumnsOfInterest
      );

   void _CreateGarbageBean(uint32_t a_Column, uint32_t a_Row);

   uint32_t _CalculateGarbageBeanNumber(uint32_t a_Score);

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

   // We must wait in the settle time state for a minimum amount of time
   int32_t m_TotalSettleTime;
   const uint32_t m_MinSettleTime;

   uint32_t m_RoundScore;

   uint32_t m_GarbageAcumulator;
   bool m_GarbageDropped;

};

#endif
