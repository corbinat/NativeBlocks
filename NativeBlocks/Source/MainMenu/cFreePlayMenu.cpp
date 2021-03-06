#include "cFreePlayMenu.h"
#include "Common/Widgets/cSelectionBox.h"
#include "Common/Widgets/cButton.h"

#include "../cAiPersonality.h"

#include <iostream>

cFreePlayMenu::cFreePlayMenu(cResources* a_pResources)
   : cObject(a_pResources),
     m_Active(false),
     m_Player1Label(),
     m_Player2Label(),
     m_GameSpeedLabel(),
     m_pPlayer1Option(NULL),
     m_pPlayer2Option(NULL),
     m_pGameSpeedOption(NULL),
     m_pStartButton(NULL),
     m_pBackButton(NULL),
     m_PostBackMessage(false)
{
   SetType("FreePlayMenu");

   SetSolid(false);

   std::shared_ptr<sf::Font> l_Font
      = GetResources()->LoadFont("Media/junegull.ttf");

   m_Player1Label.setFont(*(l_Font.get()));
   m_Player1Label.setString("Player 1: ");
   m_Player1Label.setCharacterSize(20);
   m_Player1Label.setColor(sf::Color::Black);

   m_pPlayer1Option = new cSelectionBox(GetResources());
   m_pPlayer1Option->AddOption("Human");

   for (std::string & l_Name : cAiPersonality::GetAINames())
   {
      m_pPlayer1Option->AddOption(l_Name);
   }

   m_Player2Label.setFont(*(l_Font.get()));
   m_Player2Label.setString("Player 2: ");
   m_Player2Label.setCharacterSize(20);
   m_Player2Label.setColor(sf::Color::Black);

   m_pPlayer2Option = new cSelectionBox(GetResources());
   for (std::string & l_Name : cAiPersonality::GetAINames())
   {
      m_pPlayer2Option->AddOption(l_Name);
   }

   m_GameSpeedLabel.setFont(*(l_Font.get()));
   m_GameSpeedLabel.setString("Speed: ");
   m_GameSpeedLabel.setCharacterSize(20);
   m_GameSpeedLabel.setColor(sf::Color::Black);

   m_pGameSpeedOption = new cSelectionBox(GetResources());
   m_pGameSpeedOption->AddOption("0");
   m_pGameSpeedOption->AddOption("1");
   m_pGameSpeedOption->AddOption("2");
   m_pGameSpeedOption->AddOption("3");
   m_pGameSpeedOption->AddOption("4");
   m_pGameSpeedOption->AddOption("5");
   m_pGameSpeedOption->AddOption("6");
   m_pGameSpeedOption->AddOption("7");
   m_pGameSpeedOption->AddOption("8");
   m_pGameSpeedOption->AddOption("9");

   m_pStartButton = new cButton(GetResources());
   m_pStartButton->SetImage("Media/Title.ani", "BlankSmallButton");
   m_pStartButton->SetLabel("Begin");

   m_pBackButton = new cButton(GetResources());
   m_pBackButton->SetImage("Media/Title.ani", "BlankSmallButton");
   m_pBackButton->SetLabel("Back");

   AddChild(m_pPlayer1Option);
   AddChild(m_pPlayer2Option);
   AddChild(m_pGameSpeedOption);
   AddChild(m_pStartButton);
   AddChild(m_pBackButton);
}

cFreePlayMenu::~cFreePlayMenu()
{
   GetResources()->GetMessageDispatcher()->CancelMessages(GetUniqueId());
}

void cFreePlayMenu::SetActive(bool a_Active)
{
   m_Active = a_Active;
}

// These functions are overloaded from cObject
void cFreePlayMenu::Initialize()
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
   l_Position.x += m_pStartButton->GetBoundingBox().width + 5;
   m_pBackButton->SetPosition(l_Position, kNormal, false);

   // Receive messages when The start button is pushed.
   sMessage l_Request;
   l_Request.m_From = m_pStartButton->GetUniqueId();
   l_Request.m_Category = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Value = GetResources()->GetMessageDispatcher()->Any();

   std::function<void(sMessage)> l_MessageCallback =
      std::bind(&cFreePlayMenu::MessageReceived, this, std::placeholders::_1);

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

void cFreePlayMenu::Collision(cObject* a_pOther)
{

}

void cFreePlayMenu::Event(std::list<sf::Event> * a_pEventList)
{
}

void cFreePlayMenu::Step (uint32_t a_ElapsedMiliSec)
{
   if (GetPosition() != GetPreviousPosition())
   {
      sf::Vector3<double> l_Position = GetPosition();
      m_Player1Label.setPosition(GetPosition().x, l_Position.y + 4);
      l_Position.y += m_pPlayer1Option->GetBoundingBox().height + 5;
      m_Player2Label.setPosition(GetPosition().x, l_Position.y + 4);
      l_Position.y += m_pPlayer2Option->GetBoundingBox().height + 5;
      m_GameSpeedLabel.setPosition(GetPosition().x, l_Position.y + 4);
   }

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
      if (GetPosition().x + m_Player1Label.getLocalBounds().width < GetResources()->GetWindow()->getSize().x / 2)
      {
         SetVelocityX(0, kNormal);
      }
   }
   else if (GetVelocity().x > 0)
   {
      if (GetPosition().x > GetResources()->GetWindow()->getSize().x)
      {
         SetVelocityX(0, kNormal);
      }
   }
}

void cFreePlayMenu::Draw(const sf::Vector2<float> & a_rkInterpolationOffset)
{
   sf::Vector2<float> l_OldPosition = m_Player1Label.getPosition();
   m_Player1Label.setPosition(l_OldPosition + a_rkInterpolationOffset);
   GetResources()->GetWindow()->draw(m_Player1Label);
   m_Player1Label.setPosition(l_OldPosition);

   l_OldPosition = m_Player2Label.getPosition();
   m_Player2Label.setPosition(l_OldPosition + a_rkInterpolationOffset);
   GetResources()->GetWindow()->draw(m_Player2Label);
   m_Player2Label.setPosition(l_OldPosition);

   l_OldPosition = m_GameSpeedLabel.getPosition();
   m_GameSpeedLabel.setPosition(l_OldPosition + a_rkInterpolationOffset);
   GetResources()->GetWindow()->draw(m_GameSpeedLabel);
   m_GameSpeedLabel.setPosition(l_OldPosition);
}

void cFreePlayMenu::MessageReceived(sMessage a_Message)
{
   if (a_Message.m_From == m_pStartButton->GetUniqueId())
   {
      //~ std::cout << "Start button pressed" << std::endl;
      GetResources()->SetActiveLevel("Level1", true);

      std::string l_Player1Option = m_pPlayer1Option->GetSelectedOption();
      std::string l_Player2Option = m_pPlayer2Option->GetSelectedOption();
      std::string l_GameSpeed = m_pGameSpeedOption->GetSelectedOption();

      GetResources()->GetGameConfigData()->SetProperty("Player1", l_Player1Option);
      GetResources()->GetGameConfigData()->SetProperty("Player2", l_Player2Option);
      GetResources()->GetGameConfigData()->SetProperty("GameType", "FreePlay");
      GetResources()->GetGameConfigData()->SetProperty("GameSpeed", l_GameSpeed);

   }
   else if (a_Message.m_From == m_pBackButton->GetUniqueId())
   {
      SetVelocityX(1000, kNormal);
      m_PostBackMessage = true;
   }
}
