#include "cChallengeMenu.h"
#include "cControlsMenu.h"
#include "Common/Widgets/cSelectionBox.h"
#include "Common/Widgets/cButton.h"

#include <iostream>

cChallengeMenu::cChallengeMenu(cResources* a_pResources)
   : cObject(a_pResources),
     m_Active(false),
     m_pContinueButton(NULL),
     m_pNewGameButton(NULL),
     m_pHighScoresButton(NULL),
     m_pBackButton(NULL),
     m_PostBackMessage(false)
{
   SetType("cChallengeMenu");

   SetSolid(false);

   std::shared_ptr<sf::Font> l_Font
      = GetResources()->LoadFont("Media/junegull.ttf");

   m_pContinueButton = new cButton(GetResources());
   m_pContinueButton->SetImage("Media/Title.ani", "BlankMediumButton");
   m_pContinueButton->SetLabel("Continue");

   m_pNewGameButton = new cButton(GetResources());
   m_pNewGameButton->SetImage("Media/Title.ani", "BlankMediumButton");
   m_pNewGameButton->SetLabel("New Game");

   m_pHighScoresButton = new cButton(GetResources());
   m_pHighScoresButton->SetImage("Media/Title.ani", "BlankMediumButton");
   m_pHighScoresButton->SetLabel("High Scores");

   m_pBackButton = new cButton(GetResources());
   m_pBackButton->SetImage("Media/Title.ani", "BlankMediumButton");
   m_pBackButton->SetLabel("Back");

   // Try loading a save file for testing
   // (*(GetResources()->GetGameConfigData()))["Challenge"].LoadFile("Config/SaveGame.SG");

   if (_SaveFileExists())
   {
      AddChild(m_pContinueButton);
   }
   AddChild(m_pNewGameButton);
   AddChild(m_pHighScoresButton);
   AddChild(m_pBackButton);
}

cChallengeMenu::~cChallengeMenu()
{
   GetResources()->GetMessageDispatcher()->CancelMessages(GetUniqueId());
}

void cChallengeMenu::SetActive(bool a_Active)
{
   m_Active = a_Active;
}

// These functions are overloaded from cObject
void cChallengeMenu::Initialize()
{
   sf::Vector3<double> l_Position = GetPosition();

   m_pContinueButton->SetPosition(l_Position, kNormal, false);
   if (_SaveFileExists())
   {

      l_Position.y += m_pContinueButton->GetBoundingBox().height + 5;
   }

   m_pNewGameButton->SetPosition(l_Position, kNormal, false);
   l_Position.y += m_pNewGameButton->GetBoundingBox().height + 5;
   m_pHighScoresButton->SetPosition(l_Position, kNormal, false);
   l_Position.y += m_pHighScoresButton->GetBoundingBox().height + 5;
   m_pBackButton->SetPosition(l_Position, kNormal, false);

   // Need to Receive messages when the Control buttons are pushed. Buttons emit
   // a "Widget" category, so we'll just look for all of those as a shortcut.
   // This is kind of fragile. Need to change buttons to just have a callback
   sMessage l_Request;
   l_Request.m_From = GetResources()->GetMessageDispatcher()->AnyID();
   l_Request.m_Category = "Widget";
   l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Value = GetResources()->GetMessageDispatcher()->Any();

   std::function<void(sMessage)> l_MessageCallback =
      std::bind(&cChallengeMenu::MessageReceived, this, std::placeholders::_1);

   GetResources()->GetMessageDispatcher()->RegisterForMessages(
      GetUniqueId(),
      l_MessageCallback,
      l_Request
      );
}

void cChallengeMenu::Collision(cObject* a_pOther)
{

}

void cChallengeMenu::Event(std::list<sf::Event> * a_pEventList)
{
}

void cChallengeMenu::Step (uint32_t a_ElapsedMiliSec)
{
   // We can't post a message from inside the MessageReceived function, so it
   // gets deferred to here instead.
   // TODO: This might not actually be true. It might only be registering for
   // messages that is problematic.
   if (m_PostBackMessage)
   {
      sMessage l_Message;
      l_Message.m_From = GetUniqueId();
      l_Message.m_Category = "Button";
      l_Message.m_Key = "Menu Change";
      l_Message.m_Value = "cMainMenu";
      GetResources()->GetMessageDispatcher()->PostMessage(l_Message);
      m_PostBackMessage = false;
   }

   if (GetVelocity().x < 0)
   {
      if (!m_Active)
      {
         if (GetPosition().x < (0 - static_cast<int32_t>(m_pNewGameButton->GetBoundingBox().width)))
         {
            SetVelocityX(0, kNormal);
            std::cout << "POW1" << std::endl;
         }
      }
      else
      {
         if (m_pNewGameButton->GetPosition().x + m_pNewGameButton->GetBoundingBox().width/2 < GetResources()->GetWindow()->getSize().x / 2)
         {
            SetVelocityX(0, kNormal);
            std::cout << "POW2" << std::endl;
         }
      }
   }
   else if (GetVelocity().x > 0)
   {
      if (!m_Active)
      {
         if (GetPosition().x > GetResources()->GetWindow()->getSize().x)
         {
            SetVelocityX(0, kNormal);
            std::cout << "POW3" << std::endl;
         }
      }
      else
      {
         if (m_pNewGameButton->GetPosition().x + m_pNewGameButton->GetBoundingBox().width/2 > GetResources()->GetWindow()->getSize().x / 2)
         {
            SetVelocityX(0, kNormal);
         }
      }
   }
}

void cChallengeMenu::Draw()
{

}

void cChallengeMenu::MessageReceived(sMessage a_Message)
{
   if (a_Message.m_From == m_pContinueButton->GetUniqueId())
   {
      //(*(GetResources()->GetGameConfigData()))["Challenge"].LoadFile("Config/SaveGame.SG");
      GetResources()->GetGameConfigData()->SetProperty("GameType", "Challenge");
      GetResources()->SetActiveLevel("Challenge", true);
   }
   else if (a_Message.m_From == m_pNewGameButton->GetUniqueId())
   {
      // Reset everything
      (*(GetResources()->GetGameConfigData()))["Challenge"]["NextAINumber"] = "0";
      (*(GetResources()->GetGameConfigData()))["Challenge"]["LastWinner"] = "";
      (*(GetResources()->GetGameConfigData()))["Challenge"]["Retries"] = "0";
      (*(GetResources()->GetGameConfigData()))["Challenge"]["RetriesHighScore"] = "0";
      (*(GetResources()->GetGameConfigData()))["Challenge"]["AINumberHighScore"] = "0";
      (*(GetResources()->GetGameConfigData()))["Challenge"]["GameStarted"] = "0";
      (*(GetResources()->GetGameConfigData()))["Challenge"]["ChallengeInProgress"] = "1";
      // Save progress for testing
      // (*(GetResources()->GetGameConfigData()))["Challenge"].ExportToFile("Config/SaveGame.SG");

      GetResources()->GetGameConfigData()->SetProperty("GameType", "Challenge");
      GetResources()->SetActiveLevel("Challenge", true);
   }
   else if (a_Message.m_From == m_pHighScoresButton->GetUniqueId())
   {
      GetResources()->SetActiveLevel("HighScores", true);
   }
   else if (a_Message.m_From == m_pBackButton->GetUniqueId())
   {
      SetVelocityX(1000, kNormal);
      m_PostBackMessage = true;
      m_Active = false;
   }
   else if (a_Message.m_Key == "Menu Change" && a_Message.m_Value == "cChallengeMenu")
   {
      SetVelocityX(1000, kNormal);
      m_Active = true;
   }
}

bool cChallengeMenu::_SaveFileExists()
{
   return static_cast<std::string>((*(GetResources()->GetGameConfigData()))["Challenge"]["ChallengeInProgress"]) == "1";
}
