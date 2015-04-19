#ifndef ___cEventCapture_h___
#define ___cEventCapture_h___

#include "cObject.h"

#include <SFML/Audio.hpp>

class cEventCapture: public cObject
{
public:

   cEventCapture(std::string a_Action, cResources* a_pResources);
   ~cEventCapture();

   // These functions are overloaded from cObject
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step (uint32_t a_ElapsedMiliSec);
   void Draw();

private:

   std::string m_Action;

};

#endif
