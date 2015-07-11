#include "cPause.h"

cPause::cPause(cResources* a_pResources)
   : cObject(a_pResources)
{
   SetType("Pause");
   SetSolid(false);
   LoadAnimations("Media/Floor.ani");
   PlayAnimationLoop("Pause");
}

cPause::~cPause()
{
}

void cPause::Event(std::list<sf::Event> * a_pEventList)
{
}

void cPause::Step (uint32_t a_ElapsedMiliSec)
{

}

void cPause::Collision(cObject* a_pOther)
{
}
