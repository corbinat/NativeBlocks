// A player is one person's playing surface. This is a 6x12 grid and contains
// children objects, such as the beans and floor

#ifndef ___cFloor_h___
#define ___cFloor_h___

#include "cObject.h"

class cFloor: public cObject
{
public:

   cFloor(cResources* a_pResources);
   virtual ~cFloor();

   // These functions are overloaded from cObject
   virtual void Event(std::list<sf::Event> * a_pEventList);
   virtual void Step (uint32_t a_ElapsedMiliSec);
   virtual void Collision(cObject* a_pOther);


private:

};

#endif
