#include "cBonusShot.h"

static const double g_kPI = 3.14159265359;
static const double g_kInitialSpeed = 500;
static const double g_kMaxSpeed = 2500;
static const double g_kAcceleration = 2000;
static const double g_kRotationSpeed = 3 * g_kPI;

cBonusShot::cBonusShot(sf::Vector3<double> a_Destination, cResources* a_pResources)
   : cObject(a_pResources),
     m_Destination(a_Destination),
     m_AngleInRadians(0),
     m_Speed(g_kInitialSpeed)
{
   SetType("BonusShot");
   SetSolid(false);
   LoadAnimations("Media/BonusShot.ani");
   PlayAnimationLoop("BonusShot");
}

cBonusShot::~cBonusShot()
{
}

void cBonusShot::Event(std::list<sf::Event> * a_pEventList)
{
}

void cBonusShot::Step (uint32_t a_ElapsedMiliSec)
{
   // Calculate angle to destination
   double l_AngleToDest =
      atan2(m_Destination.y - GetPosition().y, m_Destination.x - GetPosition().x);

   if (m_AngleInRadians < l_AngleToDest + g_kPI / 40 || m_AngleInRadians > l_AngleToDest - g_kPI / 40)
   {
      // We need to rotate our angle towards destination. Figure out direction.
      bool l_IsClockwise = false;
      if (l_AngleToDest < -g_kPI / 2.0)
      {
         l_IsClockwise = true;
      }

      double l_AngleDiff = l_AngleToDest - m_AngleInRadians;
      l_AngleDiff = std::fmod(l_AngleDiff, 2 * g_kPI);
      //~ if (l_AngleDiff > 0)
      //~ {
         //~ l_IsClockwise = false;
      //~ }
//~
      //~ if (l_AngleDiff > g_kPI)
      //~ {
         //~ l_IsClockwise = !l_IsClockwise;
      //~ }

      // Now rotate
      if (m_Speed < g_kMaxSpeed)
      {
         m_Speed += g_kAcceleration * a_ElapsedMiliSec / 1000;
      }
      else
      {
         std::cout << "MAXXXXXXXXXXXX" << std::endl;
      }

      double l_DistanceToRotate = m_Speed / 1000 * 4 * g_kPI * a_ElapsedMiliSec / 1000.0;
      if (l_DistanceToRotate > fabs(l_AngleDiff))
      {
         m_AngleInRadians = l_AngleToDest;
      }
      else if (l_IsClockwise)
      {
         m_AngleInRadians -= l_DistanceToRotate;
         if (m_AngleInRadians < -g_kPI)
         {
            m_AngleInRadians += 2 * g_kPI;
         }
      }
      else
      {
         m_AngleInRadians += l_DistanceToRotate;
         if (m_AngleInRadians > g_kPI)
         {
            m_AngleInRadians -= 2 * g_kPI;
         }
      }

   }

   SetVelocityX(cos(m_AngleInRadians) * m_Speed, kNormal);
   SetVelocityY(sin(m_AngleInRadians) * m_Speed, kNormal);

   // If the destination is between our position and original position then we
   // made it.
   sf::Vector3<double> l_Distance1 = m_Destination - m_OriginalPosition;
   sf::Vector3<double> l_Distance2 = GetPosition() - m_OriginalPosition;
   if (fabs(l_Distance2.x) > fabs(l_Distance1.x))
   {
      UnregisterObject(true);
   }

}

void cBonusShot::Collision(cObject* a_pOther)
{
}

void cBonusShot::Initialize()
{
   m_OriginalPosition = GetPosition();

   // Initial angle starts away from destination
   m_AngleInRadians = atan2(m_Destination.y - m_OriginalPosition.y, m_Destination.x - m_OriginalPosition.x);
   m_AngleInRadians += g_kPI;

   SetVelocityX(cos(m_AngleInRadians) * m_Speed, kNormal);
   SetVelocityY(sin(m_AngleInRadians) * m_Speed, kNormal);
}

