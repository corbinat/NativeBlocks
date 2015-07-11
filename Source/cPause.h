#ifndef ___cPause_h___
#define ___cPause_h___

#include "cObject.h"

class cPause: public cObject
{
public:

   cPause(cResources* a_pResources);
   virtual ~cPause();

   // These functions are overloaded from cObject
   virtual void Event(std::list<sf::Event> * a_pEventList);
   virtual void Step (uint32_t a_ElapsedMiliSec);
   virtual void Collision(cObject* a_pOther);


private:

};

#endif
