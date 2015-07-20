
#ifndef ___cChallengeMenu_h___
#define ___cChallengeMenu_h___

#include "cObject.h"

class cControlsMenu;
class cButton;

class cChallengeMenu: public cObject
{
public:

   cChallengeMenu(cResources* a_pResources);
   ~cChallengeMenu();

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
   bool _SaveFileExists();

   bool m_Active;
   cButton * m_pContinueButton;
   cButton * m_pNewGameButton;
   cButton * m_pHighScoresButton;
   cButton * m_pBackButton;

   bool m_PostBackMessage;

};

#endif
