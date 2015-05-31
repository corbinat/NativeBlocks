// A player is one person's playing surface. This is a 6x12 grid and contains
// children objects, such as the beans and floor

#ifndef ___cPortrait_h___
#define ___cPortrait_h___

#include "cObject.h"

class cPortrait: public cObject
{
public:

   cPortrait(cResources* a_pResources, std::string a_PlayerName);
   virtual ~cPortrait();

   // These functions are overloaded from cObject
   virtual void Initialize();
   virtual void Event(std::list<sf::Event> * a_pEventList);
   virtual void Step (uint32_t a_ElapsedMiliSec);
   virtual void Draw();
   virtual void Collision(cObject* a_pOther);


private:

};

#endif
