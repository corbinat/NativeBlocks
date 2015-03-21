#include "cChallengeDisplay.h"
#include "cAiBlock.h"
#include "cArrow.h"
#include "Common/Widgets/cButton.h"

#include "../cAiPersonality.h"

#include <iostream>

static const uint32_t g_kDelayToBlockFall = 1500;
static const uint32_t g_kDelayToContinue = 2300;

cChallengeDisplay::cChallengeDisplay(cResources* a_pResources)
   : cObject(a_pResources),
     m_NewBlockNeeded(false),
     m_TimeAccumulator(0),
     m_BlockStack(),
     m_ContinueString(),
     m_ReadyToContinue(false),
     m_pStartButton(NULL),
     m_pBackButton(NULL)
{
   SetType("ChallegeDisplay");

   SetSolid(false);

   std::shared_ptr<sf::Font> l_Font
      = GetResources()->LoadFont("Media/junegull.ttf");

   std::vector<std::string> l_AINameVector = cAiPersonality::GetAINames();
   std::string l_NextAINumberString =
      (*(GetResources()->GetGameConfigData()))["Challenge"].GetProperty("NextAINumber");
   if (l_NextAINumberString.empty())
   {
      l_NextAINumberString = "0";
   }

   uint32_t l_NextAINumber = std::stoi(l_NextAINumberString);

   // Create the blocks
   for (uint32_t i = 0; i < l_NextAINumber+1; ++i)
   {
      cAiBlock * l_NewBlock = new cAiBlock(GetResources());
      l_NewBlock->SetDepth(-5);
      sf::Vector3<double> l_Position(
         GetResources()->GetWindow()->getSize().x/2 - (l_NewBlock->GetBoundingBox().width/2),
         420 - (i * l_NewBlock->GetBoundingBox().height),
         0
         );

      l_NewBlock->SetPosition(l_Position, kNormal, false);
      l_NewBlock->SetAiLabel(l_AINameVector[i]);
      l_NewBlock->Initialize();
      l_NewBlock->SetDepth(i);

      m_BlockStack.push_back(l_NewBlock);
   }

   // Create the arrow at the top block
   cArrow * l_pNewArrow = new cArrow(GetResources());
   sf::Vector3<double> l_Position(
      m_BlockStack.back()->GetPosition().x - l_pNewArrow->GetBoundingBox().width - 50,
      m_BlockStack.back()->GetPosition().y + (m_BlockStack.back()->GetBoundingBox().height / 2) - (l_pNewArrow->GetBoundingBox().height / 2),
      0
      );
   l_pNewArrow->SetPosition(l_Position);
   l_pNewArrow->Initialize();

   // See if the Human won the last game (player 1). If so, move on to the next
   // AI.
   std::string l_LastWinner =
      (*(GetResources()->GetGameConfigData()))["Challenge"].GetProperty("LastWinner");

   if (l_LastWinner == "Player1")
   {
      ++l_NextAINumber;
      l_NextAINumberString = std::to_string(l_NextAINumber);
      (*(GetResources()->GetGameConfigData()))["Challenge"].SetProperty("LastWinner", "");
      (*(GetResources()->GetGameConfigData()))["Challenge"].SetProperty("NextAINumber", l_NextAINumberString);

      // Create another block that falls from the sky
      m_NewBlockNeeded = true;

   }
   else
   {
      l_pNewArrow->SetBounce(true);
   }

   std::string l_ChallengeMessage = "Challenge ";
   l_ChallengeMessage += l_NextAINumberString + ": ";
   l_ChallengeMessage += l_AINameVector[l_NextAINumber];

   m_ContinueString.setFont(*(l_Font.get()));
   m_ContinueString.setString("");
   m_ContinueString.setCharacterSize(20);
   m_ContinueString.setColor(sf::Color::White);

   m_pStartButton = new cButton(GetResources());
   m_pStartButton->SetImage("Media/Title.ani", "BeginButton");
   m_pStartButton->SetSolid(false);

   m_pBackButton = new cButton(GetResources());
   m_pBackButton->SetImage("Media/Title.ani", "BackButton");

   AddChild(m_pStartButton);
   AddChild(m_pBackButton);
}

cChallengeDisplay::~cChallengeDisplay()
{
   GetResources()->GetMessageDispatcher()->CancelMessages(GetUniqueId());
}

// These functions are overloaded from cObject
void cChallengeDisplay::Initialize()
{
   sf::Vector3<double> l_Position = GetPosition();
   m_ContinueString.setPosition(l_Position.x, l_Position.y);

   l_Position.y += 50;

   m_pStartButton->SetPosition(l_Position, kNormal, false);
   l_Position.x += m_pStartButton->GetBoundingBox().width + 5;
   m_pBackButton->SetPosition(l_Position, kNormal, false);

   // Receive messages when The start button is pushed.
   sMessage l_Request;
   l_Request.m_From = m_pStartButton->GetUniqueId();
   l_Request.m_Category = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Value = GetResources()->GetMessageDispatcher()->Any();

   std::function<void(sMessage)> l_MessageCallback =
      std::bind(&cChallengeDisplay::MessageReceived, this, std::placeholders::_1);

   GetResources()->GetMessageDispatcher()->RegisterForMessages(
      GetUniqueId(),
      l_MessageCallback,
      l_Request
      );

   // Receive messages when the back button is pushed.
   l_Request.m_From = m_pBackButton->GetUniqueId();
   l_Request.m_Category = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Value = GetResources()->GetMessageDispatcher()->Any();

   GetResources()->GetMessageDispatcher()->RegisterForMessages(
      GetUniqueId(),
      l_MessageCallback,
      l_Request
      );
}

void cChallengeDisplay::Collision(cObject* a_pOther)
{

}

void cChallengeDisplay::Event(std::list<sf::Event> * a_pEventList)
{
   // TODO: This is DEBUGGING
   for (std::list<sf::Event>::iterator i = a_pEventList->begin();
      i != a_pEventList->end();
      ++i
      )
   {
      switch((*i).type)
      {
         case sf::Event::KeyPressed:
         {

            if (m_ReadyToContinue)
            {
               GetResources()->SetActiveLevel("Level1", true);

               GetResources()->GetGameConfigData()->SetProperty("Player1", "Human");

               std::string l_NextAINumber = (*(GetResources()->GetGameConfigData()))["Challenge"].GetProperty("NextAINumber");
               if (l_NextAINumber.empty())
               {
                  l_NextAINumber = "0";
               }

               GetResources()->GetGameConfigData()->SetProperty(
                  "Player2",
                  cAiPersonality::GetAINames()[std::stoi(l_NextAINumber)]
                  );
            }


            if ((*i).key.code == sf::Keyboard::Num0)
            {
               (*(GetResources()->GetGameConfigData()))["Challenge"].SetProperty("NextAINumber", "0");
            }
            else if ((*i).key.code == sf::Keyboard::Num1)
            {
               (*(GetResources()->GetGameConfigData()))["Challenge"].SetProperty("NextAINumber", "1");
            }
            else if ((*i).key.code == sf::Keyboard::Num2)
            {
               (*(GetResources()->GetGameConfigData()))["Challenge"].SetProperty("NextAINumber", "2");
            }
            else if ((*i).key.code == sf::Keyboard::A)
            {
                (*(GetResources()->GetGameConfigData()))["Challenge"].SetProperty("LastWinner", "Player1");
            }
            break;
         }
      }
   }


}

void cChallengeDisplay::Step (uint32_t a_ElapsedMiliSec)
{
   if (m_TimeAccumulator >= g_kDelayToBlockFall && m_NewBlockNeeded)
   {
      cAiBlock * l_NewBlock = new cAiBlock(GetResources());
      sf::Vector3<double> l_Position(
         GetResources()->GetWindow()->getSize().x/2 - (l_NewBlock->GetBoundingBox().width/2),
         0 - l_NewBlock->GetBoundingBox().height,
         0
         );

      std::string l_NextAINumberString =
         (*(GetResources()->GetGameConfigData()))["Challenge"].GetProperty("NextAINumber");
      if (l_NextAINumberString.empty())
      {
         l_NextAINumberString = "0";
      }

      uint32_t l_NextAINumber = std::stoi(l_NextAINumberString);

      l_NewBlock->SetPosition(l_Position, kNormal, false);
      l_NewBlock->SetAiLabel(cAiPersonality::GetAINames()[l_NextAINumber]);
      l_NewBlock->Initialize();
      l_NewBlock->Fall();
      l_NewBlock->SetDepth(l_NextAINumber);

      m_NewBlockNeeded = false;
   }
   else if (m_TimeAccumulator >= g_kDelayToContinue && !m_ReadyToContinue)
   {
      m_ReadyToContinue = true;
      m_ContinueString.setString("Press any button to continue");
      m_ContinueString.setPosition(
         GetResources()->GetWindow()->getSize().x - m_ContinueString.getLocalBounds().width - 10,
         GetResources()->GetWindow()->getSize().y - m_ContinueString.getCharacterSize() - 10
         );
   }
   else
   {
      m_TimeAccumulator += a_ElapsedMiliSec;
   }

}

void cChallengeDisplay::Draw()
{
   GetResources()->GetWindow()->draw(m_ContinueString);
}

void cChallengeDisplay::MessageReceived(sMessage a_Message)
{
   if (a_Message.m_From == m_pStartButton->GetUniqueId())
   {
      GetResources()->SetActiveLevel("Level1", true);

      GetResources()->GetGameConfigData()->SetProperty("Player1", "Human");

      std::string l_NextAINumber = (*(GetResources()->GetGameConfigData()))["Challenge"].GetProperty("NextAINumber");
      if (l_NextAINumber.empty())
      {
         l_NextAINumber = "0";
      }

      GetResources()->GetGameConfigData()->SetProperty(
         "Player2",
         cAiPersonality::GetAINames()[std::stoi(l_NextAINumber)]
         );

   }
   else if (a_Message.m_From == m_pBackButton->GetUniqueId())
   {
      GetResources()->SetActiveLevel("MainMenu", true);
   }
}
