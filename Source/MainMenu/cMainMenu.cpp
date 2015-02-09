#include "cMainMenu.h"
#include "cSelectionBox.h"
#include "cButton.h"

#include <iostream>

cMainMenu::cMainMenu(cResources* a_pResources)
   : cObject(a_pResources),
     m_Player1Label(),
     m_Player2Label(),
     m_GameSpeedLabel(),
     m_pPlayer1Option(NULL),
     m_pPlayer2Option(NULL),
     m_pGameSpeedOption(NULL),
     m_pStartButton(NULL)
{
   SetType("MainMenu");

   SetSolid(false);

   std::shared_ptr<sf::Font> l_Font
      = GetResources()->LoadFont("Media/junegull.ttf");

   m_Player1Label.setFont(*(l_Font.get()));
   m_Player1Label.setString("Player 1: ");
   m_Player1Label.setCharacterSize(20);
   m_Player1Label.setColor(sf::Color::Black);

   m_pPlayer1Option = new cSelectionBox(GetResources());
   m_pPlayer1Option->AddOption("Human");
   m_pPlayer1Option->AddOption("Beginner AI");
   m_pPlayer1Option->AddOption("Easy AI");
   m_pPlayer1Option->AddOption("Medium AI");
   m_pPlayer1Option->AddOption("Hard AI");

   m_Player2Label.setFont(*(l_Font.get()));
   m_Player2Label.setString("Player 2: ");
   m_Player2Label.setCharacterSize(20);
   m_Player2Label.setColor(sf::Color::Black);

   m_pPlayer2Option = new cSelectionBox(GetResources());
   m_pPlayer2Option->AddOption("Beginner AI");
   m_pPlayer2Option->AddOption("Easy AI");
   m_pPlayer2Option->AddOption("Medium AI");
   m_pPlayer2Option->AddOption("Hard AI");

   m_GameSpeedLabel.setFont(*(l_Font.get()));
   m_GameSpeedLabel.setString("Speed: ");
   m_GameSpeedLabel.setCharacterSize(20);
   m_GameSpeedLabel.setColor(sf::Color::Black);

   m_pGameSpeedOption = new cSelectionBox(GetResources());
   m_pGameSpeedOption->AddOption("1");
   m_pGameSpeedOption->AddOption("2");
   m_pGameSpeedOption->AddOption("3");

   m_pStartButton = new cButton(GetResources());
   m_pStartButton->SetImage("Media/Title.ani", "BeginButton");
}

cMainMenu::~cMainMenu()
{
   GetResources()->GetMessageDispatcher()->CancelMessages(GetUniqueId());
}

// These functions are overloaded from cObject
void cMainMenu::Initialize()
{
   sf::Vector3<double> l_Position = GetPosition();
   m_Player1Label.setPosition(GetPosition().x, l_Position.y + 4);
   l_Position.x += 90;
   m_pPlayer1Option->SetPosition(l_Position, kNormal, false);
   m_pPlayer1Option->Initialize();

   l_Position.y += m_pPlayer1Option->GetBoundingBox().height + 5;
   m_Player2Label.setPosition(GetPosition().x, l_Position.y + 4);
   m_pPlayer2Option->SetPosition(l_Position, kNormal, false);
   m_pPlayer2Option->Initialize();

   l_Position.y += m_pPlayer2Option->GetBoundingBox().height + 5;
   m_GameSpeedLabel.setPosition(GetPosition().x, l_Position.y + 4);
   m_pGameSpeedOption->SetPosition(l_Position, kNormal, false);
   m_pGameSpeedOption->Initialize();

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
}

void cMainMenu::Collision(cObject* a_pOther)
{

}

void cMainMenu::Event(std::list<sf::Event> * a_pEventList)
{
}

void cMainMenu::Step (uint32_t a_ElapsedMiliSec)
{

}

void cMainMenu::Draw()
{
   GetResources()->GetWindow()->draw(m_Player1Label);
   GetResources()->GetWindow()->draw(m_Player2Label);
   GetResources()->GetWindow()->draw(m_GameSpeedLabel);
}

void cMainMenu::MessageReceived(sMessage a_Message)
{
   if (a_Message.m_From == m_pStartButton->GetUniqueId())
   {
      //~ std::cout << "Start button pressed" << std::endl;
      GetResources()->SetActiveLevel("Level1", true);

      std::string l_Player1Option = m_pPlayer1Option->GetSelectedOption();
      std::string l_Player2Option = m_pPlayer2Option->GetSelectedOption();

      GetResources()->GetGameConfigData()->SetProperty("Player1", l_Player1Option);
      GetResources()->GetGameConfigData()->SetProperty("Player2", l_Player2Option);

   }
}
