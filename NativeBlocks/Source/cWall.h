#ifndef ___cWall_h___
#define ___cWall_h___

#include "cObject.h"

class cWall: public cObject
{
public:

   cWall(cResources* a_pResources);
   virtual ~cWall();

   // These functions are overloaded from cObject
   virtual void Event(std::list<sf::Event> * a_pEventList);
   virtual void Step (uint32_t a_ElapsedMiliSec);
   virtual void Collision(cObject* a_pOther);


private:

};

#endif
