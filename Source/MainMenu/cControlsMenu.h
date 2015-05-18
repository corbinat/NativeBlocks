
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

   enum eActions
   {
      kActionDOwn,
      kActionLeft,
      kActionRight,
      kActionClockwise,
      kActionCounterClockwise,
      kActionCount
   };

   void _UpdateButtons();

   // The pair is the button label and the button itself
   std::vector< std::pair<sf::Text, cButton*> > m_ActionButtons;
   cButton * m_pBackButton;

   bool m_PostBackMessage;

};

#endif
