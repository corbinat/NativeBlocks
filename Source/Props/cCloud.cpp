#include "cCloud.h"
#include <iostream>

cCloud::cCloud(cResources* a_pResources)
   : cObject(a_pResources),
     m_Speed(5)
{
   SetType("Cloud");
   SetSolid(true);
   LoadAnimations("Media/Title.ani");
   PlayAnimationLoop("Cloud1");
   SetDepth(1, kNormal);
   SetCollidable(false);
}

cCloud::~cCloud()
{

}

// These functions are overloaded from cObject
void cCloud::Initialize()
{
   m_Speed += GetPosition().y/17;
   SetVelocityX(m_Speed, kNormal);
}

void cCloud::Collision(cObject* a_pOther)
{

}

void cCloud::Event(std::list<sf::Event> * a_pEventList)
{
}

void cCloud::Step (uint32_t a_ElapsedMiliSec)
{
   if (GetPosition().x > GetResources()->GetWindow()->getSize().x)
   {
      //std::cout << "Resetting" << std::endl;
      sf::Vector3<double> l_NewPosition(
         0 - static_cast<double>(GetBoundingBox().width),
         GetPosition().y,
         0
         );

      SetPosition(l_NewPosition, kNormal, false);
   }
}

void cCloud::Draw()
{

}
