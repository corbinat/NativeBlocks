
#ifndef ___cChallengeDisplay_h___
#define ___cChallengeDisplay_h___

#include "cArrow.h"
#include "cObject.h"

enum eChallengeState
{
   kStateStart,
   kStateDelayToBlockFall,
   kStateBlockFall,
   kStateWaitForFallToFinish,
   kStateMoveArrowUp,
   kStateDelayToContinue,
   kStateReadyToContinue
};

class cButton;

class cChallengeDisplay: public cObject
{
public:

   cChallengeDisplay(cResources* a_pResources);
   ~cChallengeDisplay();

   // These functions are overloaded from cObject
   void Initialize();
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step (uint32_t a_ElapsedMiliSec);
   void Draw(const sf::Vector2<float> & a_rkInterpolationOffset);

   // Used by child widgets to communicate to this parent widget
   void MessageReceived(sMessage a_Message);

private:

   eChallengeState m_State;
   uint32_t m_TimeAccumulator;
   std::vector<cObject*> m_BlockStack;
   cArrow* m_pArrow;
   cView m_View;

   sf::Text m_ContinueString;
};

#endif
