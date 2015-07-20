#include "cGrass.h"
#include <iostream>

cGrass::cGrass(cResources* a_pResources)
   : cObject(a_pResources)
{
   SetType("Grass");
   SetSolid(true);
   LoadAnimations("Media/Title.ani");
   PlayAnimationLoop("Grass");
}

cGrass::~cGrass()
{

}

// These functions are overloaded from cObject
void cGrass::Collision(cObject* a_pOther)
{

}

void cGrass::Event(std::list<sf::Event> * a_pEventList)
{
}

void cGrass::Step (uint32_t a_ElapsedMiliSec)
{
}

void cGrass::Draw(const sf::Vector2<float> & a_rkInterpolationOffset)
{

}
