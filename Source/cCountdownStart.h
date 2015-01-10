#ifndef ___cCountdownStart_h___
#define ___cCountdownStart_h___

#include "cObject.h"

class cCountdownStart: public cObject
{
public:

   cCountdownStart(cResources* a_pResources);
   virtual ~cCountdownStart();

   // These functions are overloaded from cObject
   virtual void Event(std::list<sf::Event> * a_pEventList);
   virtual void Step (uint32_t a_ElapsedMiliSec);
   virtual void Collision(cObject* a_pOther);


private:

   uint32_t m_TimeAlive;

};

#endif
