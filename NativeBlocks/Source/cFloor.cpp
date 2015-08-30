#include "cFloor.h"

cFloor::cFloor(cResources* a_pResources)
   : cObject(a_pResources)
{
   SetType("Floor");
   SetSolid(true);
   SetCollidable(true);
   LoadAnimations("Media/Floor.ani");
   PlayAnimationLoop("Floor");
}

cFloor::~cFloor()
{
}

void cFloor::Event(std::list<sf::Event> * a_pEventList)
{
}

void cFloor::Step (uint32_t a_ElapsedMiliSec)
{

}

void cFloor::Collision(cObject* a_pOther)
{
}
