#include "cSpeedController.h"
#include <iostream>

cSpeedController::cSpeedController(uint32_t a_Level)
   : m_StartingMsPerFall(500),
     m_FastestMsPerFall(0),
     m_CurrentMsPerFall(500),
     m_ChangeRate(0),
     m_Accumulator(0)
{

   uint32_t l_Starting = GetStartingMsPerFallAtLevel(a_Level);
   switch(a_Level)
   {
      case 0:
      {
         m_StartingMsPerFall = l_Starting;
         m_CurrentMsPerFall = l_Starting;
         m_FastestMsPerFall = l_Starting - 100;

         // 5 minues / 100 ticks = 3000 ms/tick
         m_ChangeRate = 3000;
         break;
      }
      case 1:
      {
         m_StartingMsPerFall = l_Starting;
         m_CurrentMsPerFall = l_Starting;
         m_FastestMsPerFall = l_Starting - 100;

         // 5 minues / 100 ticks = 3000 ms/tick
         m_ChangeRate = 3000;
         break;
      }
      case 2:
      {
         m_StartingMsPerFall = l_Starting;
         m_CurrentMsPerFall = l_Starting;
         m_FastestMsPerFall = l_Starting - 100;

         // 5 minues / 100 ticks = 3000 ms/tick
         m_ChangeRate = 3000;
         break;
      }
      case 3:
      {
         m_StartingMsPerFall = l_Starting;
         m_CurrentMsPerFall = l_Starting;
         m_FastestMsPerFall = l_Starting - 100;

         // 5 minues / 100 ticks = 3000 ms/tick
         m_ChangeRate = 3000;
         break;
      }
      case 4:
      {
         m_StartingMsPerFall = l_Starting;
         m_CurrentMsPerFall = l_Starting;
         m_FastestMsPerFall = l_Starting - 100;

         // 5 minues / 100 ticks = 3000 ms/tick
         m_ChangeRate = 3000;
         break;
      }
      case 5:
      {
         m_StartingMsPerFall = l_Starting;
         m_CurrentMsPerFall = l_Starting;
         m_FastestMsPerFall = l_Starting - 100;

         // 5 minues / 100 ticks = 3000 ms/tick
         m_ChangeRate = 3000;
         break;
      }
      case 6:
      {
         m_StartingMsPerFall = l_Starting;
         m_CurrentMsPerFall = l_Starting;
         m_FastestMsPerFall = l_Starting - 100;

         // 5 minues / 100 ticks = 3000 ms/tick
         m_ChangeRate = 3000;
         break;
      }
      case 7:
      {
         m_StartingMsPerFall = l_Starting;
         m_CurrentMsPerFall = l_Starting;
         m_FastestMsPerFall = l_Starting - 100;

         // 5 minues / 100 ticks = 3000 ms/tick
         m_ChangeRate = 3000;
         break;
      }
      case 8:
      {
         m_StartingMsPerFall = l_Starting;
         m_CurrentMsPerFall = l_Starting;
         m_FastestMsPerFall = 50;

         // 1 minues / 50 ticks = 1200 ms/tick
         m_ChangeRate = 1200;
         break;
      }
      case 9:
      {
         m_StartingMsPerFall = l_Starting;
         m_CurrentMsPerFall = l_Starting;
         m_FastestMsPerFall = 30;

         // 1 minues / 50 ticks = 1200 ms/tick
         m_ChangeRate = 1200;
         break;
      }
      default:
      {
         m_StartingMsPerFall = l_Starting;
         m_CurrentMsPerFall = l_Starting;
         m_FastestMsPerFall = l_Starting - 100;

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

uint32_t cSpeedController::GetStartingMsPerFallAtLevel(uint32_t a_Level)
{
   switch (a_Level)
   {
      case 0:
      {
         return 500;
      }
      case 1:
      {
         return 450;
      }
      case 2:
      {
         return 400;
      }
      case 3:
      {
         return 350;
      }
      case 4:
      {
         return 300;
      }
      case 5:
      {
         return 250;
      }
      case 6:
      {
         return 200;
      }
      case 7:
      {
         return 150;
      }
      case 8:
      {
         return 100;
      }
      case 9:
      {
         return 70;
      }
      default:
      {
         return 100;
      }
   }
}
