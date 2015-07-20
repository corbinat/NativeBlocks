
#ifndef ___cTitle_h___
#define ___cTitle_h___

#include "cObject.h"

#include <SFML/Audio.hpp>

class cTitle: public cObject
{
public:

   cTitle(cResources* a_pResources);
   ~cTitle();

   // These functions are overloaded from cObject
   void Initialize();
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step (uint32_t a_ElapsedMiliSec);
   void Draw(const sf::Vector2<float> & a_rkInterpolationOffset);

private:

   sf::Text m_DemoString;
};

#endif
