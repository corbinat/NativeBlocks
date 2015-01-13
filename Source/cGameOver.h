#ifndef ___cGameOver_h___
#define ___cGameOver_h___

#include "cObject.h"

class cGameOver: public cObject
{
public:

   cGameOver(cResources* a_pResources);
   virtual ~cGameOver();

   // These functions are overloaded from cObject
   virtual void Event(std::list<sf::Event> * a_pEventList);
   virtual void Step (uint32_t a_ElapsedMiliSec);
   virtual void Collision(cObject* a_pOther);


private:

};

#endif
