
#ifndef ___cControlsMenu_h___
#define ___cControlsMenu_h___

#include "cObject.h"

class cSelectionBox;
class cButton;

class cControlsMenu: public cObject
{
public:

   cControlsMenu(cResources* a_pResources);
   ~cControlsMenu();

   // These functions are overloaded from cObject
   void Initialize();
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step (uint32_t a_ElapsedMiliSec);
   void Draw();

   // Used by child widgets to communicate to this parent widget
   void MessageReceived(sMessage a_Message);

private:
   sf::Text m_DownLabel;
   cButton * m_pDownButton;
   cButton * m_pBackButton;

   bool m_PostBackMessage;

};

#endif
