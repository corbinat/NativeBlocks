#include "cEventCapture.h"

#include <iostream>

cEventCapture::cEventCapture(std::string a_Action, cResources* a_pResources)
   : cObject(a_pResources),
     m_Action(a_Action)
{
   std::cout << "Creating Event Capture" << std::endl;
   SetType("EventCapture");
   SetSolid(false);

   // Depth needs to be high so it receives events first.
   SetDepth(100);
   // LoadAnimations("Media/Title.ani");
   // PlayAnimationLoop("TitleBanner");
}

cEventCapture::~cEventCapture()
{

}

// These functions are overloaded from cObject
void cEventCapture::Collision(cObject* a_pOther)
{

}

void cEventCapture::Event(std::list<sf::Event> * a_pEventList)
{
   std::list<sf::Event>::iterator i = a_pEventList->begin();
   for (i; i != a_pEventList->end(); ++i)
   {
      std::cout << "Adding" << std::endl;
      bool l_Success =
         GetResources()->GetEventTranslator()->AddBinding(*i, m_Action);
      if (l_Success)
      {
         std::cout << "Binding Added" << std::endl;
         UnregisterObject(true);
         break;
      }
   }

   // Eat all events so that other objects don't react to them
   a_pEventList->clear();
}

void cEventCapture::Step (uint32_t a_ElapsedMiliSec)
{
}

void cEventCapture::Draw()
{

}
