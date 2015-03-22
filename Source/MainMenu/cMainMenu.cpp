#include "cMainMenu.h"

#include "Common/Widgets/cButton.h"
#include "cFreePlayMenu.h"

#include <iostream>

cMainMenu::cMainMenu(cResources* a_pResources)
   : cObject(a_pResources),
     m_pChallengeButton(NULL),
     m_pFreePlayButton(NULL),
     m_pFreePlayMenu(NULL)
{
   SetType("MainMenu");

   SetSolid(false);

   m_pChallengeButton = new cButton(GetResources());
   m_pChallengeButton->SetImage("Media/Title.ani", "BlankMediumButton");
   m_pChallengeButton->SetLabel("Challenge");

   m_pFreePlayButton = new cButton(GetResources());
   m_pFreePlayButton->SetImage("Media/Title.ani", "BlankMediumButton");
   m_pFreePlayButton->SetLabel("Free Play");

   m_pFreePlayMenu = new cFreePlayMenu(GetResources());

   AddChild(m_pChallengeButton);
   AddChild(m_pFreePlayButton);
   AddChild(m_pFreePlayMenu);
}

cMainMenu::~cMainMenu()
{
   GetResources()->GetMessageDispatcher()->CancelMessages(GetUniqueId());
}

// These functions are overloaded from cObject
void cMainMenu::Initialize()
{
   sf::Vector3<double> l_Position = GetPosition();
   m_pChallengeButton->SetPosition(l_Position, kNormal, false);
   m_pChallengeButton->Initialize();

   l_Position.y += m_pChallengeButton->GetBoundingBox().height + 5;

   m_pFreePlayButton->SetPosition(l_Position, kNormal, false);
   m_pFreePlayButton->Initialize();

   // Receive messages when The Challenege button is pushed.
   std::function<void(sMessage)> l_MessageCallback =
      std::bind(&cMainMenu::MessageReceived, this, std::placeholders::_1);

   sMessage l_Request;
   l_Request.m_From = m_pChallengeButton->GetUniqueId();
   l_Request.m_Category = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Value = GetResources()->GetMessageDispatcher()->Any();

   GetResources()->GetMessageDispatcher()->RegisterForMessages(
      GetUniqueId(),
      l_MessageCallback,
      l_Request
      );

   // Receive messages when the Freeplay button is pushed
   l_Request.m_From = m_pFreePlayButton->GetUniqueId();
   l_Request.m_Category = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Value = GetResources()->GetMessageDispatcher()->Any();

   GetResources()->GetMessageDispatcher()->RegisterForMessages(
      GetUniqueId(),
      l_MessageCallback,
      l_Request
      );

   // Receive messages for when this menu should be active
   l_Request.m_From = GetResources()->GetMessageDispatcher()->AnyID();
   l_Request.m_Category = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Key = "Menu Change";
   l_Request.m_Value = "cMainMenu";

   GetResources()->GetMessageDispatcher()->RegisterForMessages(
      GetUniqueId(),
      l_MessageCallback,
      l_Request
      );

   // Set up the other menus
   l_Position = GetPosition();
   l_Position.x = GetResources()->GetWindow()->getSize().x;
   //~ l_Position.x = GetResources()->GetWindow()->getSize().x - 200;
   m_pFreePlayMenu->SetPosition(l_Position, kNormal, false);
   m_pFreePlayMenu->Initialize();
}

void cMainMenu::Collision(cObject* a_pOther)
{

}

void cMainMenu::Event(std::list<sf::Event> * a_pEventList)
{
}

void cMainMenu::Step (uint32_t a_ElapsedMiliSec)
{
   if (GetVelocity().x < 0)
   {
      if (GetPosition().x < (0 - static_cast<int32_t>(m_pChallengeButton->GetBoundingBox().width)))
      {
         SetVelocityX(0, kNormal);
      }
   }
   else if (GetVelocity().x > 0)
   {
      if (m_pFreePlayMenu->GetPosition().x > 800)
      {
         SetVelocityX(0, kNormal);
      }
   }
}

void cMainMenu::Draw()
{

}

void cMainMenu::MessageReceived(sMessage a_Message)
{
   if (a_Message.m_From == m_pChallengeButton->GetUniqueId())
   {
      GetResources()->GetGameConfigData()->SetProperty("GameType", "Challenge");
      GetResources()->SetActiveLevel("Challenge", true);
      std::cout << "Challenge" << std::endl;
   }
   else if (a_Message.m_From == m_pFreePlayButton->GetUniqueId())
   {
      SetVelocityX(-700, kNormal);
   }
   else if (a_Message.m_Key == "Menu Change" && a_Message.m_Value == "cMainMenu")
   {
      SetVelocityX(700, kNormal);
   }
}
