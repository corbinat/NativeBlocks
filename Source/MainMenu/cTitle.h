
#ifndef ___cTitle_h___
#define ___cTitle_h___

#include "cObject.h"

class cTitle: public cObject
{
public:

   cTitle(cResources* a_pResources);
   ~cTitle();

   // These functions are overloaded from cObject
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step (uint32_t a_ElapsedMiliSec);
   void Draw();

private:
};

#endif
