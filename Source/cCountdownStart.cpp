#include "cCountdownStart.h"
#include <iostream>

cCountdownStart::cCountdownStart(cResources* a_pResources)
   : cObject(a_pResources),
     m_Started(false),
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
   if (!m_Started && m_TimeAlive > 300)
   {
      m_Started = true;
      m_TimeAlive = 0;
      LoadAnimations("Media/Countdown.ani");
      PlayAnimationLoop("Countdown");
   }
   else if (m_Started && m_TimeAlive > 2000)
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
