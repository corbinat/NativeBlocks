#include "cWall.h"

cWall::cWall(cResources* a_pResources)
   : cObject(a_pResources)
{
   SetType("Wall");
   SetSolid(true);
   SetCollidable(true);
   LoadAnimations("Media/Floor.ani");
   PlayAnimationLoop("Wall");
}

cWall::~cWall()
{
}

void cWall::Event(std::list<sf::Event> * a_pEventList)
{
}

void cWall::Step (uint32_t a_ElapsedMiliSec)
{

}

void cWall::Collision(cObject* a_pOther)
{
}
