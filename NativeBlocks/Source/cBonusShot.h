// A player is one person's playing surface. This is a 6x12 grid and contains
// children objects, such as the beans and floor

#ifndef ___cBonusShot_h___
#define ___cBonusShot_h___

#include "cObject.h"

class cBonusShot: public cObject
{
public:

   cBonusShot(sf::Vector3<double> a_Destination, cResources* a_pResources);
   virtual ~cBonusShot();

   // These functions are overloaded from cObject
   virtual void Event(std::list<sf::Event> * a_pEventList);
   virtual void Step (uint32_t a_ElapsedMiliSec);
   virtual void Collision(cObject* a_pOther);
   virtual void Initialize();


private:

   sf::Vector3<double> m_Destination;
   sf::Vector3<double> m_OriginalPosition;

   double m_AngleInRadians;
   double m_Speed;

};

#endif
