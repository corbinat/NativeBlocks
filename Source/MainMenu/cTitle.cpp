#include "cTitle.h"

#include <iostream>

cTitle::cTitle(cResources* a_pResources)
   : cObject(a_pResources)
{
   SetType("TitleBox");
   SetSolid(true);
   LoadAnimations("Media/Title.ani");
   PlayAnimationLoop("TitleBanner");

   // The title plays the music
   GetResources()->GetBackGroundMusic()->openFromFile("Media/Music/Intro.ogg");
   GetResources()->GetBackGroundMusic()->setLoop(true);
   GetResources()->GetBackGroundMusic()->play();
}

cTitle::~cTitle()
{

}

// These functions are overloaded from cObject
void cTitle::Collision(cObject* a_pOther)
{

}

void cTitle::Event(std::list<sf::Event> * a_pEventList)
{
}

void cTitle::Step (uint32_t a_ElapsedMiliSec)
{
}

void cTitle::Draw()
{

}
