
#ifndef ___cFreePlayMenu_h___
#define ___cFreePlayMenu_h___

#include "cObject.h"

class cSelectionBox;
class cButton;

class cFreePlayMenu: public cObject
{
public:

   cFreePlayMenu(cResources* a_pResources);
   ~cFreePlayMenu();

   void SetActive(bool a_Active);

   // These functions are overloaded from cObject
   void Initialize();
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step (uint32_t a_ElapsedMiliSec);
   void Draw(const sf::Vector2<float> & a_rkInterpolationOffset);

   // Used by child widgets to communicate to this parent widget
   void MessageReceived(sMessage a_Message);

private:
   bool m_Active;

   sf::Text m_Player1Label;
   sf::Text m_Player2Label;
   sf::Text m_GameSpeedLabel;

   cSelectionBox * m_pPlayer1Option;
   cSelectionBox * m_pPlayer2Option;
   cSelectionBox * m_pGameSpeedOption;
   cButton * m_pStartButton;
   cButton * m_pBackButton;

   bool m_PostBackMessage;

};

#endif
