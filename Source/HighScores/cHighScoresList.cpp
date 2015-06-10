#include "cHighScoresList.h"

#include "cVFileNode.h"

#include <iostream>

cHighScoresList::cHighScoresList(cResources* a_pResources)
   : cObject(a_pResources),
     m_Title(),
     m_HighScores(11),
     m_ContinueString()
{
   SetType("HighScoreList");
   SetSolid(false);
   SetCollidable(false);

   LoadAnimations("Media/HighScoreBg.ani");
   PlayAnimationLoop("HighScoreBg");

   sf::Vector3<double> l_Position = GetPosition();
   l_Position.x = GetResources()->GetWindow()->getSize().x/2 - (GetBoundingBox().width/2);
   l_Position.y = GetResources()->GetWindow()->getSize().y/2 - (GetBoundingBox().height/2);
   SetPosition(l_Position, kNormal, false);



   std::shared_ptr<sf::Font> l_Font
      = GetResources()->LoadFont("Media/junegull.ttf");

   // Set up the title
   m_Title.setFont(*(l_Font.get()));
   m_Title.setString("High Scores");
   m_Title.setCharacterSize(30);
   m_Title.setColor(sf::Color::Black);

   // Set up the list
   for (size_t i = 0; i < m_HighScores.size(); ++i)
   {
      m_HighScores[i].m_Name.setFont(*(l_Font.get()));
      m_HighScores[i].m_Level.setFont(*(l_Font.get()));
      m_HighScores[i].m_Retries.setFont(*(l_Font.get()));
   }

   m_HighScores[0].m_Name.setString("Name");
   m_HighScores[0].m_Level.setString("Level");
   m_HighScores[0].m_Retries.setString("Retries");

   // Set up the continue message
   m_ContinueString.setFont(*(l_Font.get()));
   m_ContinueString.setString("");
   m_ContinueString.setCharacterSize(18);
   m_ContinueString.setColor(sf::Color::White);
   m_ContinueString.setString("Press any button to continue");

   _EvaluateHighScoreList();
}

cHighScoresList::~cHighScoresList()
{

}

// These functions are overloaded from cObject

void cHighScoresList::Initialize()
{
   sf::Vector3<double> l_Position = GetPosition();

   l_Position.x += (GetBoundingBox().width/2) - m_Title.getLocalBounds().width / 2;

   m_Title.setPosition(l_Position.x, l_Position.y);

   l_Position.y += m_Title.getCharacterSize() * 2;

   for (size_t i = 0; i < m_HighScores.size(); ++i)
   {
      l_Position.x = GetPosition().x + 100;
      m_HighScores[i].m_Name.setPosition(l_Position.x, l_Position.y);

      l_Position.x += 120;
      m_HighScores[i].m_Level.setPosition(l_Position.x, l_Position.y);

      l_Position.x += 120;
      m_HighScores[i].m_Retries.setPosition(l_Position.x, l_Position.y);

      l_Position.y += m_HighScores[i].m_Name.getCharacterSize();

      if (i == 0)
      {
         l_Position.y += m_HighScores[i].m_Name.getCharacterSize();
      }
   }

   // Set the position relative to the camera
   float l_X = GetResources()->GetWindow()->getSize().x - m_ContinueString.getLocalBounds().width - 10;
   float l_Y = GetResources()->GetWindow()->getSize().y - m_ContinueString.getCharacterSize() - 10;
   m_ContinueString.setPosition(l_X, l_Y);
}

void cHighScoresList::Collision(cObject* a_pOther)
{

}

void cHighScoresList::Event(std::list<sf::Event> * a_pEventList)
{
   for (std::list<sf::Event>::iterator i = a_pEventList->begin();
      i != a_pEventList->end();
      ++i
      )
   {
      switch((*i).type)
      {
         case sf::Event::KeyPressed:
         case sf::Event::JoystickButtonPressed:
         {
            GetResources()->SetActiveLevel("MainMenu", true);
            break;
         }
      }
   }
}

void cHighScoresList::Step (uint32_t a_ElapsedMiliSec)
{
}

void cHighScoresList::Draw()
{
   GetResources()->GetWindow()->draw(m_Title);

   for (auto i = m_HighScores.begin(); i != m_HighScores.end(); ++i)
   {
      GetResources()->GetWindow()->draw((*i).m_Name);
      GetResources()->GetWindow()->draw((*i).m_Level);
      GetResources()->GetWindow()->draw((*i).m_Retries);
   }

   GetResources()->GetWindow()->draw(m_ContinueString);
}

void cHighScoresList::_EvaluateHighScoreList()
{
   std::shared_ptr<sf::Font> l_Font
      = GetResources()->LoadFont("Media/junegull.ttf");

   cVFileNode l_HighScores;
   l_HighScores.LoadFile("Config/HighScores.cfg");

   for (size_t i = 0; i < l_HighScores["HighScores"].Size(); ++i)
   {
      m_HighScores[i+1].m_Name.setString(static_cast<std::string>(l_HighScores["HighScores"][i]["Name"]));
      m_HighScores[i+1].m_Level.setString(static_cast<std::string>(l_HighScores["HighScores"][i]["Level"]));
      m_HighScores[i+1].m_Retries.setString(static_cast<std::string>(l_HighScores["HighScores"][i]["Retries"]));
   }

   // Don't try to save the score if the game is still in progress
   if (static_cast<std::string>((*(GetResources()->GetGameConfigData()))["Challenge"]["ChallengeInProgress"]) == "1")
   {
      return;
   }

   // See if the user needs to insert something into the list
   std::string l_UserRetriesString =
      static_cast<std::string>((*(GetResources()->GetGameConfigData()))["Challenge"]["RetriesHighScore"]);
   if (l_UserRetriesString == "") l_UserRetriesString = "0";

   std::string l_UserAINumberString =
      static_cast<std::string>((*(GetResources()->GetGameConfigData()))["Challenge"]["AINumberHighScore"]);
   if (l_UserAINumberString == "") l_UserAINumberString = "0";

   std::string l_PlayerName =
      static_cast<std::string>((*(GetResources()->GetGameConfigData()))["Challenge"]["PlayerName"]);

   bool l_Updated = false;
   for (size_t i = 1; i < m_HighScores.size(); ++i)
   {
      std::string l_Level = m_HighScores[i].m_Level.getString();
      std::string l_Retries = m_HighScores[i].m_Retries.getString();

      if (  std::stoi(l_UserAINumberString) > std::stoi(l_Level)
         || (std::stoi(l_UserAINumberString) == std::stoi(l_Level) && std::stoi(l_UserRetriesString) < std::stoi(l_Retries)))
      {
         // User needs to be inserted into this position.
         sHighScore l_NewHighScore;
         l_NewHighScore.m_Name.setFont(*(l_Font.get()));
         l_NewHighScore.m_Level.setFont(*(l_Font.get()));
         l_NewHighScore.m_Retries.setFont(*(l_Font.get()));
         l_NewHighScore.m_Name.setString(l_PlayerName);
         l_NewHighScore.m_Level.setString(l_UserAINumberString);
         l_NewHighScore.m_Retries.setString(l_UserRetriesString);
         // + 1 because the first line is the labels
         m_HighScores.insert(m_HighScores.begin() + i, l_NewHighScore);
         m_HighScores.resize(11);

         l_Updated = true;
         break;
      }
   }

   // If we updated we also need to save to file and clear the status
   if (l_Updated)
   {
      cVFileNode l_NewHighScoreFile;
      for(size_t i = 1; i < m_HighScores.size(); ++i)
      {
         l_NewHighScoreFile["HighScores"][i - 1]["Name"] = m_HighScores[i].m_Name.getString();
         l_NewHighScoreFile["HighScores"][i - 1]["Level"] = m_HighScores[i].m_Level.getString();
         l_NewHighScoreFile["HighScores"][i - 1]["Retries"] = m_HighScores[i].m_Retries.getString();
      }

      l_NewHighScoreFile.ExportToFile("Config/HighScores.cfg");

      // Clearing the score is enough to reset.
      (*(GetResources()->GetGameConfigData()))["Challenge"]["AINumberHighScore"] =
         "0";
   }

}
