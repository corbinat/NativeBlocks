#include "cPortrait.h"

cPortrait::cPortrait(cResources* a_pResources, std::string a_PlayerName)
   : cObject(a_pResources)
{
   SetType("cPortrait");
   SetSolid(false);
   SetCollidable(false);
   LoadAnimations("Media/Players.ani");
   PlayAnimationLoop(a_PlayerName);
   SetDepth(-3);
}

cPortrait::~cPortrait()
{
}

void cPortrait::Initialize()
{
}

void cPortrait::Event(std::list<sf::Event> * a_pEventList)
{
}

void cPortrait::Step (uint32_t a_ElapsedMiliSec)
{

}
void cPortrait::Draw()
{
}

void cPortrait::Collision(cObject* a_pOther)
{
}
