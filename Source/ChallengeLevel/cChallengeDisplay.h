
#ifndef ___cChallengeDisplay_h___
#define ___cChallengeDisplay_h___

#include "cObject.h"

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
   void Draw();

   // Used by child widgets to communicate to this parent widget
   void MessageReceived(sMessage a_Message);

private:

   bool m_NewBlockNeeded;
   uint32_t m_TimeAccumulator;
   std::vector<cObject*> m_BlockStack;

   sf::Text m_ContinueString;
   bool m_ReadyToContinue;

   cButton * m_pStartButton;
   cButton * m_pBackButton;
};

#endif
