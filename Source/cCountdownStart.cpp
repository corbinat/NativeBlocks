#include "cCountdownStart.h"
#include <iostream>

cCountdownStart::cCountdownStart(cResources* a_pResources)
   : cObject(a_pResources),
     m_State(kThreeState),
     m_TimeAlive(0)
{
   SetType("CountdownStart");
   SetSolid(true);
   SetCollidable(false);
}

cCountdownStart::~cCountdownStart()
{
}

void cCountdownStart::Event(std::list<sf::Event> * a_pEventList)
{
}

void cCountdownStart::Step (uint32_t a_ElapsedMiliSec)
{
   m_TimeAlive += a_ElapsedMiliSec;

   // Keep game paused for 2 seconds
   if (m_TimeAlive > 300 && m_State == kThreeState)
   {
      LoadAnimations("Media/Countdown.ani");
      PlayAnimationLoop("Three");
      m_State = kTwoState;
   }
   else if (m_TimeAlive > 900 && m_State == kTwoState)
   {
      PlayAnimationLoop("Two");
      m_State = kOneState;
   }
   else if (m_TimeAlive > 1500 && m_State == kOneState)
   {
      PlayAnimationLoop("One");
      m_State = kGoState;
   }
   else if (m_TimeAlive > 2100 && m_State == kGoState)
   {
      PlayAnimationLoop("Go");
      m_State = kWaitState;
   }
   else if (m_TimeAlive > 2700)
   {
      // Tell the players they can begin.
      sMessage l_Message;
      l_Message.m_From = GetUniqueId();
      l_Message.m_Category = "StartGame";
      l_Message.m_Key = "StartGame";
      l_Message.m_Value = "StartGame";
      GetResources()->GetMessageDispatcher()->PostMessage(l_Message);

      UnregisterObject(true);
   }

}

void cCountdownStart::Collision(cObject* a_pOther)
{
}
