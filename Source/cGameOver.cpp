#include "cGameOver.h"

cGameOver::cGameOver(cResources* a_pResources)
   : cObject(a_pResources)
{
   SetType("GameOver");
   SetSolid(true);
   LoadAnimations("Media/Final.ani");
   PlayAnimationLoop("GameOver");
}

cGameOver::~cGameOver()
{
}

void cGameOver::Event(std::list<sf::Event> * a_pEventList)
{
}

void cGameOver::Step (uint32_t a_ElapsedMiliSec)
{

}

void cGameOver::Collision(cObject* a_pOther)
{
}
