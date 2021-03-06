
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
   void Draw(const sf::Vector2<float> & a_rkInterpolationOffset);

   // Used by child widgets to communicate to this parent widget
   void MessageReceived(sMessage a_Message);

private:
   cButton * m_pChallengeButton;
   cChallengeMenu * m_pChallengeMenu;
   cButton * m_pFreePlayButton;
   cFreePlayMenu * m_pFreePlayMenu;
   cButton * m_pOptionsButton;
   cOptionsMenu * m_pOptionsMenu;

};

#endif
