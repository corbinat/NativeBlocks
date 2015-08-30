#ifndef ___cArrow_h___
#define ___cArrow_h___

#include "cObject.h"


class cArrow: public cObject
{
public:
   cArrow(cResources* a_pResources);
   ~cArrow();

   void SetBounce(bool a_Enable);

   // These functions are overloaded from cObject
   void Initialize();
   void Event(std::list<sf::Event> * a_pEventList);
   void Step(uint32_t a_ElapsedMiliSec);
   void Collision(cObject* a_pOther);
   void Draw(const sf::Vector2<float> & a_rkInterpolationOffset);


private:
   bool m_BounceEnabled;
   bool m_BouncingLeft;
   double m_Offset;

};

#endif
