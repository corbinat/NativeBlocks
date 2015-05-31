
#ifndef ___cMainMenu_h___
#define ___cMainMenu_h___

#include "cObject.h"

class cFreePlayMenu;
class cOptionsMenu;
class cChallengeMenu;
class cButton;

class cMainMenu: public cObject
{
public:

   cMainMenu(cResources* a_pResources);
   ~cMainMenu();

   // These functions are overloaded from cObject
   void Initialize();
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step (uint32_t a_ElapsedMiliSec);
   void Draw();

   // Used by child widgets to communicate to this parent widget
   void MessageReceived(sMessage a_Message);

private:
   cButton * m_pChallengeButton;
   cButton * m_pFreePlayButton;
   cButton * m_pOptionsButton;

   cFreePlayMenu * m_pFreePlayMenu;
   cOptionsMenu * m_pOptionsMenu;
   cChallengeMenu * m_pChallengeMenu;;
};

#endif
