
#ifndef ___cCloud_h___
#define ___cCloud_h___

#include "cObject.h"

class cCloud: public cObject
{
public:

   cCloud(cResources* a_pResources);
   ~cCloud();

   // These functions are overloaded from cObject
   void Initialize();
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step (uint32_t a_ElapsedMiliSec);
   void Draw(const sf::Vector2<float> & a_rkInterpolationOffset);

private:

   uint32_t m_Speed;

};

#endif
