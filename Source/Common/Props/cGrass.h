
#ifndef ___cGrass_h___
#define ___cGrass_h___

#include "cObject.h"

class cGrass: public cObject
{
public:

   cGrass(cResources* a_pResources);
   ~cGrass();

   // These functions are overloaded from cObject
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step (uint32_t a_ElapsedMiliSec);
   void Draw(const sf::Vector2<float> & a_rkInterpolationOffset);

private:
};

#endif
