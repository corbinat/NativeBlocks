
#ifndef ___cOptionsMenu_h___
#define ___cOptionsMenu_h___

#include "cObject.h"

class cButton;

class cOptionsMenu: public cObject
{
public:

   cOptionsMenu(cResources* a_pResources);
   ~cOptionsMenu();

   // These functions are overloaded from cObject
   void Initialize();
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step (uint32_t a_ElapsedMiliSec);
   void Draw();

   // Used by child widgets to communicate to this parent widget
   void MessageReceived(sMessage a_Message);

private:

   cButton * m_pKeyMapButton;
   cButton * m_pBackButton;

   bool m_PostBackMessage;

};

#endif
