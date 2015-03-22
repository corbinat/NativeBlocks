
#ifndef ___cFinalMenu_h___
#define ___cFinalMenu_h___

#include "cObject.h"

class cSelectionBox;
class cButton;

class cFinalMenu: public cObject
{
public:

   cFinalMenu(cResources* a_pResources);
   ~cFinalMenu();

   // These functions are overloaded from cObject
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step (uint32_t a_ElapsedMiliSec);
   void Draw();

   // Used by child widgets to communicate to this parent widget
   void MessageReceived(sMessage a_Message);

private:

   void _CreateFinalFreePlayMenu();
   void _CreateFinalChallengeMenu();

   bool m_NeedToActivateButtons;

   cButton * m_pPlayAgainButton;
   cButton * m_pMainMenuButton;

};

#endif
