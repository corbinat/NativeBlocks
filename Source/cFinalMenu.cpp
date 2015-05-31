#include "cFinalMenu.h"
#include "Common/Widgets/cButton.h"

#include <iostream>

cFinalMenu::cFinalMenu(cResources* a_pResources)
   : cObject(a_pResources),
     m_pPlayAgainButton(NULL),
     m_pMainMenuButton(NULL),
     m_NeedToActivateButtons(false)
{
   SetType("FinalMenu");

   SetSolid(false);

   // Receive messages for when a player loses. That's our time to shine
   sMessage l_Request;
   l_Request.m_From = GetResources()->GetMessageDispatcher()->AnyID();
   l_Request.m_Category = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Value = "Player Lost";

   std::function<void(sMessage)> l_MessageCallback =
      std::bind(&cFinalMenu::MessageReceived, this, std::placeholders::_1);

   GetResources()->GetMessageDispatcher()->RegisterForMessages(
      GetUniqueId(),
      l_MessageCallback,
      l_Request
      );
}

cFinalMenu::~cFinalMenu()
{
   GetResources()->GetMessageDispatcher()->CancelMessages(GetUniqueId());
}

// These functions are overloaded from cObject
void cFinalMenu::Collision(cObject* a_pOther)
{

}

void cFinalMenu::Event(std::list<sf::Event> * a_pEventList)
{
}

void cFinalMenu::Step (uint32_t a_ElapsedMiliSec)
{
   if(m_NeedToActivateButtons)
   {
      // Receive messages when the Play Again button is pushed.
      sMessage l_Request;
      l_Request.m_From = m_pPlayAgainButton->GetUniqueId();
      l_Request.m_Category = GetResources()->GetMessageDispatcher()->Any();
      l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
      l_Request.m_Value = GetResources()->GetMessageDispatcher()->Any();

      std::function<void(sMessage)> l_MessageCallback =
         std::bind(&cFinalMenu::MessageReceived, this, std::placeholders::_1);

      GetResources()->GetMessageDispatcher()->RegisterForMessages(
         GetUniqueId(),
         l_MessageCallback,
         l_Request
         );

      // Receive messages when the Main Menu button is pushed.
      l_Request.m_From = m_pMainMenuButton->GetUniqueId();
      l_Request.m_Category = GetResources()->GetMessageDispatcher()->Any();
      l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
      l_Request.m_Value = GetResources()->GetMessageDispatcher()->Any();

      GetResources()->GetMessageDispatcher()->RegisterForMessages(
         GetUniqueId(),
         l_MessageCallback,
         l_Request
         );

      m_NeedToActivateButtons = false;
   }

   if (m_pPlayAgainButton != NULL && m_pPlayAgainButton->GetPosition().y < GetPosition().y)
   {
      m_pPlayAgainButton->SetPosition(sf::Vector3<double>(0, 600.0 / 1000 * a_ElapsedMiliSec, 0), kRelative, false);
   }
   if (m_pMainMenuButton != NULL && m_pMainMenuButton->GetPosition().y < GetPosition().y + m_pPlayAgainButton->GetBoundingBox().height + 10)
   {
      m_pMainMenuButton->SetPosition(sf::Vector3<double>(0, 600.0 / 1000 * a_ElapsedMiliSec, 0), kRelative, false);
   }
}

void cFinalMenu::Draw()
{
}

void cFinalMenu::MessageReceived(sMessage a_Message)
{
   if (a_Message.m_Value == "Player Lost")
   {
      //~ std::cout << "Detecting player lost" << std::endl;
      if (GetResources()->GetGameConfigData()->GetProperty("GameType") == "FreePlay")
      {
         _CreateFinalFreePlayMenu();
      }
      else
      {
         // Mark the game as no longer in progress
         (*(GetResources()->GetGameConfigData()))["Challenge"]["GameStarted"] = "0";

         // If player lost then record it
         std::string l_LastWinner =
            (*(GetResources()->GetGameConfigData()))["Challenge"]["LastWinner"];

         if (l_LastWinner != "Player1")
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

         // Save progress for testing
         // (*(GetResources()->GetGameConfigData()))["Challenge"].ExportToFile("Config/SaveGame.SG");

         _CreateFinalChallengeMenu();
      }
   }
   else if (m_pPlayAgainButton != NULL && a_Message.m_From == m_pPlayAgainButton->GetUniqueId())
   {
      //~ std::cout << "Play Again pressed" << std::endl;
      if (GetResources()->GetGameConfigData()->GetProperty("GameType") == "FreePlay")
      {
         GetResources()->SetActiveLevel("Level1", true);
      }
      else
      {
         GetResources()->SetActiveLevel("Challenge", true);
      }
   }
   else if (m_pMainMenuButton != NULL && a_Message.m_From == m_pMainMenuButton->GetUniqueId())
   {
      //~ std::cout << "Main Menu button pressed" << std::endl;
      GetResources()->SetActiveLevel("MainMenu", true);
   }
}

void cFinalMenu::_CreateFinalFreePlayMenu()
{
   // If the buttons have already been created, don't do it again
   if (m_pPlayAgainButton != NULL)
   {
      return;
   }

   m_pPlayAgainButton = new cButton(GetResources());
   m_pPlayAgainButton->SetImage("Media/Final.ani", "Button");
   m_pPlayAgainButton->SetLabel("Play Again", 17);
   sf::Vector3<double> l_Position = GetPosition();
   l_Position.y = -1.0 * m_pPlayAgainButton->GetBoundingBox().height;
   m_pPlayAgainButton->SetPosition(l_Position, kNormal, false);

   m_pMainMenuButton = new cButton(GetResources());
   m_pMainMenuButton->SetImage("Media/Final.ani", "Button");
   m_pMainMenuButton->SetLabel("Main Menu", 17);
   l_Position.y -= m_pMainMenuButton->GetBoundingBox().height + 10;
   m_pMainMenuButton->SetPosition(l_Position, kNormal, false);

   m_NeedToActivateButtons = true;

}

void cFinalMenu::_CreateFinalChallengeMenu()
{
   // If the buttons have already been created, don't do it again
   if (m_pPlayAgainButton != NULL)
   {
      return;
   }

   m_pPlayAgainButton = new cButton(GetResources());
   m_pPlayAgainButton->SetImage("Media/Final.ani", "Button");
   m_pPlayAgainButton->SetLabel("Continue", 17);
   sf::Vector3<double> l_Position = GetPosition();
   l_Position.y = -1.0 * m_pPlayAgainButton->GetBoundingBox().height;
   m_pPlayAgainButton->SetPosition(l_Position, kNormal, false);

   m_pMainMenuButton = new cButton(GetResources());
   m_pMainMenuButton->SetImage("Media/Final.ani", "Button");
   m_pMainMenuButton->SetLabel("Main Menu", 17);
   l_Position.y -= m_pMainMenuButton->GetBoundingBox().height + 10;
   m_pMainMenuButton->SetPosition(l_Position, kNormal, false);

   m_NeedToActivateButtons = true;

}
