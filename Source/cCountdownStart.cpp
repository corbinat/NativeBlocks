#include "cCountdownStart.h"
#include "Common/cFadeTransition.h"
#include <iostream>

cCountdownStart::cCountdownStart(cResources* a_pResources)
   : cObject(a_pResources),
     m_State(kThreeState),
     m_TimeAlive(0)
{
   SetType("CountdownStart");
   SetSolid(true);
   SetCollidable(false);
   GetResources()->GetBackGroundMusic()->stop();
   GetResources()->PlayBackgroundMusic("Media/Music/GetIt.ogg");
   GetResources()->GetBackGroundMusic()->setLoop(true);

   cFadeTransition * l_pFadeTransition = new cFadeTransition(GetResources());
   l_pFadeTransition->SetFadeDirection(cFadeTransition::kFadeDirectionIn);
   l_pFadeTransition->SetTransitionTime(1000);
   // Make sure the transition is in front of everything
   l_pFadeTransition->SetDepth(-10);
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

   if (m_TimeAlive > 1300 && m_State == kThreeState)
   {
      LoadAnimations("Media/Countdown.ani");
      PlayAnimationLoop("Three");
      PlaySound("Media/Sounds/BlockMove.wav");
      m_State = kTwoState;
   }
   else if (m_TimeAlive > 1900 && m_State == kTwoState)
   {
      PlayAnimationLoop("Two");
      PlaySound("Media/Sounds/BlockMove.wav");
      m_State = kOneState;
   }
   else if (m_TimeAlive > 2500 && m_State == kOneState)
   {
      PlayAnimationLoop("One");
      PlaySound("Media/Sounds/BlockMove.wav");
      m_State = kGoState;
   }
   else if (m_TimeAlive > 3100 && m_State == kGoState)
   {
      PlayAnimationLoop("Go");
      PlaySound("Media/Sounds/BlockMove.wav");
      GetResources()->GetBackGroundMusic()->play();
      m_State = kWaitState;
   }
   else if (m_TimeAlive > 3700)
   {
      if (GetResources()->GetGameConfigData()->GetProperty("GameType") == "Challenge")
      {
         // Anti-cheating measure. If this is a challenge game then mark the game
         // in progress. If the player quits out then we can use this to increase
         // the retry count
         std::string l_GameStarted =
            (*(GetResources()->GetGameConfigData()))["Challenge"]["GameStarted"];
         if (l_GameStarted == "1")
         {
            std::string l_RetryString =
               (*(GetResources()->GetGameConfigData()))["Challenge"]["Retries"];
            if (l_RetryString.empty())
            {
               l_RetryString = "0";
            }
            uint32_t l_RetryCount = std::stoi(l_RetryString);
            ++l_RetryCount;
            (*(GetResources()->GetGameConfigData()))["Challenge"]["Retries"]
               = std::to_string(l_RetryCount);
            }

         (*(GetResources()->GetGameConfigData()))["Challenge"]["GameStarted"] = "1";

         // Save progress for testing
         // (*(GetResources()->GetGameConfigData()))["Challenge"].ExportToFile("Config/SaveGame.SG");
      }

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
