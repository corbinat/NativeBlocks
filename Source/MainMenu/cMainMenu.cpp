#include "cMainMenu.h"
#include "cSelectionBox.h"
#include "cButton.h"

#include <iostream>

cMainMenu::cMainMenu(cResources* a_pResources)
   : cObject(a_pResources),
     m_Initialized(false),
     m_pPlayer1Option(NULL),
     m_pPlayer2Option(NULL),
     m_pGameSpeedOption(NULL),
     m_pStartButton(NULL)
{
   SetType("MainMenu");

   SetSolid(false);

   m_pPlayer1Option = new cSelectionBox(GetResources());
   m_pPlayer1Option->AddOption("Human");
   m_pPlayer1Option->AddOption("Easy AI");
   m_pPlayer1Option->AddOption("Medium AI");
   m_pPlayer1Option->AddOption("Hard AI");

   m_pPlayer2Option = new cSelectionBox(GetResources());
   m_pPlayer2Option->AddOption("Easy AI");
   m_pPlayer2Option->AddOption("Medium AI");
   m_pPlayer2Option->AddOption("Hard AI");

   m_pGameSpeedOption = new cSelectionBox(GetResources());
   m_pGameSpeedOption->AddOption("1");
   m_pGameSpeedOption->AddOption("2");
   m_pGameSpeedOption->AddOption("3");

   m_pStartButton = new cButton(GetResources());
}

cMainMenu::~cMainMenu()
{

}

// These functions are overloaded from cObject
void cMainMenu::Collision(cObject* a_pOther)
{

}

void cMainMenu::Event(std::list<sf::Event> * a_pEventList)
{
}

void cMainMenu::Step (uint32_t a_ElapsedMiliSec)
{

   if (!m_Initialized)
   {
      sf::Vector3<double> l_Position = GetPosition();
      m_pPlayer1Option->SetPosition(l_Position, kNormal, false);

      l_Position.y += m_pPlayer1Option->GetBoundingBox().height;
      m_pPlayer2Option->SetPosition(l_Position, kNormal, false);

      l_Position.y += m_pPlayer2Option->GetBoundingBox().height;
      m_pGameSpeedOption->SetPosition(l_Position, kNormal, false);

      l_Position.y += m_pGameSpeedOption->GetBoundingBox().height + 10;
      m_pStartButton->SetPosition(l_Position, kNormal, false);

      // Receive messages when The start button is pushed.
      sMessage l_Request;
      l_Request.m_From = m_pStartButton->GetUniqueId();
      l_Request.m_Category = GetResources()->GetMessageDispatcher()->Any();
      l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
      l_Request.m_Value = GetResources()->GetMessageDispatcher()->Any();

      std::function<void(sMessage)> l_MessageCallback =
         std::bind(&cMainMenu::MessageReceived, this, std::placeholders::_1);

      GetResources()->GetMessageDispatcher()->RegisterForMessages(
         GetUniqueId(),
         l_MessageCallback,
         l_Request
         );

      m_Initialized = true;
   }

}

void cMainMenu::Draw()
{
}

void cMainMenu::MessageReceived(sMessage a_Message)
{
   if (a_Message.m_From == m_pStartButton->GetUniqueId())
   {
      std::cout << "Start button pressed" << std::endl;
      GetResources()->SetActiveLevel("Level1", true);

   }
}
