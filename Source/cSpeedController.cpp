#include "cSpeedController.h"
#include <iostream>

cSpeedController::cSpeedController(uint32_t a_Level)
   : m_StartingMsPerFall(500),
     m_FastestMsPerFall(0),
     m_CurrentMsPerFall(500),
     m_ChangeRate(0),
     m_Accumulator(0)
{
   switch(a_Level)
   {
      case 0:
      {
         m_StartingMsPerFall = 500;
         m_CurrentMsPerFall = 500;
         m_FastestMsPerFall = 400;

         // 5 minues / 100 ticks = 3000 ms/tick
         m_ChangeRate = 3000;
         break;
      }
      case 1:
      {
         m_StartingMsPerFall = 450;
         m_CurrentMsPerFall = 450;
         m_FastestMsPerFall = 350;

         // 5 minues / 100 ticks = 3000 ms/tick
         m_ChangeRate = 3000;
         break;
      }
      case 2:
      {
         m_StartingMsPerFall = 400;
         m_CurrentMsPerFall = 400;
         m_FastestMsPerFall = 300;

         // 5 minues / 100 ticks = 3000 ms/tick
         m_ChangeRate = 3000;
         break;
      }
      case 3:
      {
         m_StartingMsPerFall = 350;
         m_CurrentMsPerFall = 350;
         m_FastestMsPerFall = 250;

         // 5 minues / 100 ticks = 3000 ms/tick
         m_ChangeRate = 3000;
         break;
      }
      case 4:
      {
         m_StartingMsPerFall = 300;
         m_CurrentMsPerFall = 300;
         m_FastestMsPerFall = 200;

         // 5 minues / 100 ticks = 3000 ms/tick
         m_ChangeRate = 3000;
         break;
      }
      case 5:
      {
         m_StartingMsPerFall = 250;
         m_CurrentMsPerFall = 250;
         m_FastestMsPerFall = 150;

         // 5 minues / 100 ticks = 3000 ms/tick
         m_ChangeRate = 3000;
         break;
      }
      case 6:
      {
         m_StartingMsPerFall = 200;
         m_CurrentMsPerFall = 200;
         m_FastestMsPerFall = 100;

         // 5 minues / 100 ticks = 3000 ms/tick
         m_ChangeRate = 3000;
         break;
      }
      case 7:
      {
         m_StartingMsPerFall = 150;
         m_CurrentMsPerFall = 150;
         m_FastestMsPerFall = 50;

         // 5 minues / 100 ticks = 3000 ms/tick
         m_ChangeRate = 3000;
         break;
      }
      default:
      {
         m_StartingMsPerFall = 70;
         m_CurrentMsPerFall = 70;
         m_FastestMsPerFall = 20;

         // 5 minues / 100 ticks = 3000 ms/tick
         m_ChangeRate = 300;
         break;
      }
   }
}

cSpeedController::~cSpeedController()
{

}

uint32_t cSpeedController::GetMiliSecPerFall()
{
   return m_CurrentMsPerFall;
}

void cSpeedController::Update(uint32_t a_ElapsedMiliSec)
{
   if (m_CurrentMsPerFall <= m_FastestMsPerFall)
   {
      return;
   }

   m_Accumulator += a_ElapsedMiliSec;
   while (m_Accumulator >= m_ChangeRate)
   {
      --m_CurrentMsPerFall;
      m_Accumulator -= m_ChangeRate;
   }
}
