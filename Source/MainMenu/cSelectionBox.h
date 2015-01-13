
#ifndef ___cSelectionBox_h___
#define ___cSelectionBox_h___

#include "cObject.h"

class cButton;
class cTextBox;

class cSelectionBox: public cObject
{
public:

   cSelectionBox(cResources* a_pResources);
   ~cSelectionBox();

   // These functions are overloaded from cObject
   void Initialize();
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step (uint32_t a_ElapsedMiliSec);
   void Draw();

   // Used by child widgets to communicate to this parent widget
   void MessageReceived(sMessage a_Message);

   void SetOptions(std::vector<std::string> & a_rOptions);
   void AddOption(const std::string & a_rkOption);
   std::string GetSelectedOption();

private:

   cButton * m_pLeftArrowButton;
   cButton * m_pRightArrowButton;
   cTextBox * m_pTextBox;

   std::vector<std::string> m_Options;
   std::vector<std::string>::iterator m_CurrentOption;

};

#endif
