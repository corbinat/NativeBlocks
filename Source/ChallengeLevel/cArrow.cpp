#include "cArrow.h"

cArrow::cArrow(cResources* a_pResources)
   : cObject(a_pResources),
     m_BounceEnabled(false),
     m_BouncingLeft(true),
     m_Offset(0)
{
   SetType("cArrow");
   SetSolid(false);
   SetCollidable(false);

   LoadAnimations("Media/Challenge.ani");
   PlayAnimationLoop("StaticArrow");
}

cArrow::~cArrow()
{

}

void cArrow::SetBounce(bool a_Enable)
{
   m_BounceEnabled = a_Enable;
}

// These functions are overloaded from cObject
void cArrow::Initialize()
{
}
void cArrow::Event(std::list<sf::Event> * a_pEventList)
{
}
void cArrow::Step(uint32_t a_ElapsedMiliSec)
{
   if (m_BounceEnabled)
   {
      if (m_Offset < -10)
      {
         m_BouncingLeft = false;
      }
      else if (m_Offset > 0)
      {
         m_BouncingLeft = true;
      }

      if (m_BounceEnabled && m_BouncingLeft)
      {
         m_Offset -= 50 * a_ElapsedMiliSec/1000.0;
         sf::Vector3<double> l_Position;
         l_Position.x -= 50 * a_ElapsedMiliSec/1000.0;
         SetPosition(l_Position, kRelative);
      }
      else if (m_BounceEnabled && !m_BouncingLeft)
      {
         m_Offset += 50 * a_ElapsedMiliSec/1000.0;
         sf::Vector3<double> l_Position;
         l_Position.x += 50 * a_ElapsedMiliSec/1000.0;
         SetPosition(l_Position, kRelative);
      }
   }
}
void cArrow::Collision(cObject* a_pOther)
{
}
void cArrow::Draw()
{
}
