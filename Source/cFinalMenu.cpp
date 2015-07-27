#include "cFinalMenu.h"
#include "cAiPersonality.h"
#include "cHighScoreRecorder.h"
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

      std::function<void(sMessage)> l_MessageCallback =
         std::bind(&cFinalMenu::MessageReceived, this, std::placeholders::_1);

      if (m_pPlayAgainButton != NULL)
      {
         l_Request.m_From = m_pPlayAgainButton->GetUniqueId();
         l_Request.m_Category = GetResources()->GetMessageDispatcher()->Any();
         l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
         l_Request.m_Value = GetResources()->GetMessageDispatcher()->Any();

         GetResources()->GetMessageDispatcher()->RegisterForMessages(
            GetUniqueId(),
            l_MessageCallback,
            l_Request
            );
      }

      // Receive messages when the Main Menu button is pushed.
      if (m_pMainMenuButton != NULL)
      {
         l_Request.m_From = m_pMainMenuButton->GetUniqueId();
         l_Request.m_Category = GetResources()->GetMessageDispatcher()->Any();
         l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
         l_Request.m_Value = GetResources()->GetMessageDispatcher()->Any();

         GetResources()->GetMessageDispatcher()->RegisterForMessages(
            GetUniqueId(),
            l_MessageCallback,
            l_Request
            );
      }

      m_NeedToActivateButtons = false;
   }

   int32_t l_VerticalOffset = 0;
   if (m_pPlayAgainButton != NULL && m_pPlayAgainButton->GetPosition().y < GetPosition().y)
   {
      m_pPlayAgainButton->SetPosition(sf::Vector3<double>(0, 600.0 / 1000 * a_ElapsedMiliSec, 0), kRelative, false);
   }

   if (m_pMainMenuButton != NULL)
   {
      if (m_pPlayAgainButton != NULL)
      {
         l_VerticalOffset = m_pPlayAgainButton->GetBoundingBox().height;
      }

      if (m_pMainMenuButton->GetPosition().y < GetPosition().y + l_VerticalOffset + 10)
      {
         m_pMainMenuButton->SetPosition(sf::Vector3<double>(0, 600.0 / 1000 * a_ElapsedMiliSec, 0), kRelative, false);
      }
      else
      {
         sf::Vector3<double> l_Position = m_pMainMenuButton->GetPosition();
         if (m_pPlayAgainButton != NULL)
         {
            l_Position = m_pPlayAgainButton->GetPosition();
            l_Position.y += m_pPlayAgainButton->GetBoundingBox().height + 11;
         }
         m_pMainMenuButton->SetPosition(l_Position, kNormal, false);
      }
   }
}

void cFinalMenu::Draw(const sf::Vector2<float> & a_rkInterpolationOffset)
{
}

void cFinalMenu::MessageReceived(sMessage a_Message)
{
   if (a_Message.m_Value == "Player Lost")
   {
      //~ std::cout << "Detecting player lost" << std::endl;
      if (GetResources()->GetGameConfigData()->GetProperty("GameType") == "FreePlay")
      {
         // For Testing
         //GetResources()->SetActiveLevel("Level1", true);

         _CreateFinalFreePlayMenu();
      }
      else
      {
         // Mark the game as no longer in progress
         (*(GetResources()->GetGameConfigData()))["Challenge"]["GameStarted"] = "0";

         // This will get set to non-zero if the player won. We can then use the
         // number to see if the player has beaten all AIs.
         uint32_t l_NextAiNumber = 0;

         // See if player won or lost
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
            (*(GetResources()->GetGameConfigData()))["Challenge"]["Retries"] =
               std::to_string(l_RetryCount);
         }
         else
         {
            // Player won. Record for the high score list
            (*(GetResources()->GetGameConfigData()))["Challenge"]["RetriesHighScore"] =
               (*(GetResources()->GetGameConfigData()))["Challenge"]["Retries"];

            // We can't use NextAINumber because it doesn't get incremented until
            // the next time we get to the cChallengeDisplay. For high score
            // purposes we need to increment something now.
            std::string l_NextAIString =
               (*(GetResources()->GetGameConfigData()))["Challenge"]["NextAINumber"];

            if (l_NextAIString.empty())
            {
               l_NextAIString = "0";
            }

            l_NextAiNumber = std::stoi(l_NextAIString);
            l_NextAiNumber++;

            (*(GetResources()->GetGameConfigData()))["Challenge"]["AINumberHighScore"] =
               std::to_string(l_NextAiNumber);
         }

         // Save progress for testing
         // (*(GetResources()->GetGameConfigData()))["Challenge"].ExportToFile("Config/SaveGame.SG");

         // See if the player has beaten all of the AIs.
         int32_t l_TotalAIs = cAiPersonality::GetAINames().size();
         if (l_NextAiNumber >= l_TotalAIs)
         {
            _CreateChallengeVictoryMenu();
         }
         else
         {
            _CreateFinalChallengeMenu();
         }
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

      if (GetResources()->GetGameConfigData()->GetProperty("GameType") == "FreePlay")
      {
         GetResources()->SetActiveLevel("MainMenu", true);
      }
      else
      {
         if (_IsHighScore())
         {
            cHighScoreRecorder * l_HighScoreRecorder =
               new cHighScoreRecorder(GetResources());
         }
         else
         {
            GetResources()->SetActiveLevel("HighScores", true);
         }

         (*(GetResources()->GetGameConfigData()))["Challenge"]["ChallengeInProgress"] = "0";
      }
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

   sf::Vector3<double> l_Position(
      GetResources()->GetWindow()->getSize().x/2 - (m_pPlayAgainButton->GetBoundingBox().width/2),
      -1.0 * m_pPlayAgainButton->GetBoundingBox().height,
      0
      );

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

   sf::Vector3<double> l_Position(
      GetResources()->GetWindow()->getSize().x/2 - (m_pPlayAgainButton->GetBoundingBox().width/2),
      -1.0 * m_pPlayAgainButton->GetBoundingBox().height,
      0
      );

   m_pPlayAgainButton->SetPosition(l_Position, kNormal, false);

   m_pMainMenuButton = new cButton(GetResources());
   m_pMainMenuButton->SetImage("Media/Final.ani", "Button");
   m_pMainMenuButton->SetLabel("Give Up", 17);
   l_Position.y -= m_pMainMenuButton->GetBoundingBox().height + 10;
   m_pMainMenuButton->SetPosition(l_Position, kNormal, false);

   m_NeedToActivateButtons = true;

}

void cFinalMenu::_CreateChallengeVictoryMenu()
{
   // If the buttons have already been created, don't do it again
   if (m_pMainMenuButton != NULL)
   {
      return;
   }

   m_pMainMenuButton = new cButton(GetResources());
   m_pMainMenuButton->SetImage("Media/Final.ani", "Button");
   m_pMainMenuButton->SetLabel("Finish", 17);

   sf::Vector3<double> l_Position(
      GetResources()->GetWindow()->getSize().x/2 - (m_pMainMenuButton->GetBoundingBox().width/2),
      -1.0 * m_pMainMenuButton->GetBoundingBox().height,
      0
      );

   m_pMainMenuButton->SetPosition(l_Position, kNormal, false);

   m_NeedToActivateButtons = true;

}

bool cFinalMenu::_IsHighScore()
{
   cVFileNode l_HighScores;
   l_HighScores.LoadFile("Config/HighScores.cfg");

   // Get the user's stats
   std::string l_UserRetriesString =
      static_cast<std::string>((*(GetResources()->GetGameConfigData()))["Challenge"]["RetriesHighScore"]);
   if (l_UserRetriesString == "") l_UserRetriesString = "0";

   std::string l_UserAINumberString =
      static_cast<std::string>((*(GetResources()->GetGameConfigData()))["Challenge"]["AINumberHighScore"]);
   if (l_UserAINumberString == "") l_UserAINumberString = "0";

   // See if the stats are good enough to be on the high score list
   if (l_HighScores["HighScores"].Size() == 0 && l_UserAINumberString != "0")
   {
      // High score list doesn't exist yet, so this must be a high score.
      return true;
   }

   for (size_t i = 1; i < l_HighScores["HighScores"].Size(); ++i)
   {
      std::string l_Level = l_HighScores["HighScores"][i]["Level"];
      std::string l_Retries = l_HighScores["HighScores"][i]["Retries"];

      if (  std::stoi(l_UserAINumberString) > std::stoi(l_Level)
         || (std::stoi(l_UserAINumberString) == std::stoi(l_Level) && std::stoi(l_UserRetriesString) < std::stoi(l_Retries)))
      {
         return true;
      }
   }
   return false;
}

