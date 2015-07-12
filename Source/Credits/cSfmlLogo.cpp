#include "cSfmlLogo.h"

#include <iostream>

cSfmlLogo::cSfmlLogo(cResources* a_pResources)
   : cObject(a_pResources)
{
   SetType("Logo");
   SetSolid(false);
   SetCollidable(false);
   LoadAnimations("Media/Sfml.ani");
   PlayAnimationLoop("SFML");

}

cSfmlLogo::~cSfmlLogo()
{

}

// These functions are overloaded from cObject

void cSfmlLogo::Initialize()
{

}

void cSfmlLogo::Collision(cObject* a_pOther)
{

}

void cSfmlLogo::Event(std::list<sf::Event> * a_pEventList)
{

}

void cSfmlLogo::Step (uint32_t a_ElapsedMiliSec)
{
}

void cSfmlLogo::Draw()
{
}
