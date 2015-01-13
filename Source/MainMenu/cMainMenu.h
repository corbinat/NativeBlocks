
#ifndef ___cMainMenu_h___
#define ___cMainMenu_h___

#include "cObject.h"

class cSelectionBox;
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

   sf::Text m_Player1Label;
   sf::Text m_Player2Label;
   sf::Text m_GameSpeedLabel;

   cSelectionBox * m_pPlayer1Option;
   cSelectionBox * m_pPlayer2Option;
   cSelectionBox * m_pGameSpeedOption;
   cButton * m_pStartButton;

};

#endif
