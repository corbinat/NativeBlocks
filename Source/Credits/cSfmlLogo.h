
#ifndef ___cSfmlLogo_h___
#define ___cSfmlLogo_h___

#include "cObject.h"

class cSfmlLogo: public cObject
{
public:

   cSfmlLogo(cResources* a_pResources);
   ~cSfmlLogo();

   // These functions are overloaded from cObject
   void Initialize();
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step (uint32_t a_ElapsedMiliSec);
   void Draw();

private:

};

#endif
