#include "cRoof.h"

cRoof::cRoof(cResources* a_pResources)
   : cObject(a_pResources)
{
   SetType("Roof");
   SetSolid(false);
   SetCollidable(false);
   LoadAnimations("Media/Floor.ani");
   PlayAnimationLoop("Roof");
   SetDepth(-2);
}

cRoof::~cRoof()
{
}

void cRoof::Event(std::list<sf::Event> * a_pEventList)
{
}

void cRoof::Step (uint32_t a_ElapsedMiliSec)
{

}

void cRoof::Collision(cObject* a_pOther)
{
}
