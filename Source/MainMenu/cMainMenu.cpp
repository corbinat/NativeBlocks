#include "cMainMenu.h"

#include "Common/Widgets/cButton.h"
#include "cFreePlayMenu.h"
#include "cOptionsMenu.h"
#include "cChallengeMenu.h"

#include <iostream>

cMainMenu::cMainMenu(cResources* a_pResources)
   : cObject(a_pResources),
     m_pChallengeButton(NULL),
     m_pChallengeMenu(NULL),
     m_pFreePlayButton(NULL),
     m_pFreePlayMenu(NULL),
     m_pOptionsButton(NULL),
     m_pOptionsMenu(NULL)
{
   SetType("MainMenu");

   SetSolid(false);

   m_pChallengeButton = new cButton(GetResources());
   m_pChallengeButton->SetImage("Media/Title.ani", "BlankMediumButton");
   m_pChallengeButton->SetLabel("Challenge");

   m_pFreePlayButton = new cButton(GetResources());
   m_pFreePlayButton->SetImage("Media/Title.ani", "BlankMediumButton");
   m_pFreePlayButton->SetLabel("Free Play");

   m_pOptionsButton = new cButton(GetResources());
   m_pOptionsButton->SetImage("Media/Title.ani", "BlankMediumButton");
   m_pOptionsButton->SetLabel("Options");

   m_pChallengeMenu = new cChallengeMenu(GetResources());
   m_pFreePlayMenu = new cFreePlayMenu(GetResources());
   m_pOptionsMenu = new cOptionsMenu(GetResources());

   AddChild(m_pChallengeButton);
   AddChild(m_pFreePlayButton);
   AddChild(m_pOptionsButton);

}

cMainMenu::~cMainMenu()
{
   GetResources()->GetMessageDispatcher()->CancelMessages(GetUniqueId());
}

// These functions are overloaded from cObject
void cMainMenu::Initialize()
{
   sf::Vector3<double> l_Position = GetPosition();
   l_Position.x = GetResources()->GetWindow()->getSize().x/2 - (m_pChallengeButton->GetBoundingBox().width/2);
   SetPosition(l_Position, kNormal, false);
   //~ l_Position.x = 5;
   m_pChallengeButton->SetPosition(l_Position, kNormal, false);
   m_pChallengeButton->Initialize();

   l_Position.y += m_pChallengeButton->GetBoundingBox().height + 5;
   m_pFreePlayButton->SetPosition(l_Position, kNormal, false);
   m_pFreePlayButton->Initialize();

   l_Position.y += m_pFreePlayButton->GetBoundingBox().height + 5;
   m_pOptionsButton->SetPosition(l_Position, kNormal, false);
   m_pOptionsButton->Initialize();

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

   // Receive messages when the Options button is pushed
   l_Request.m_From = m_pOptionsButton->GetUniqueId();
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
   m_pOptionsMenu->SetPosition(l_Position, kNormal, false);
   m_pOptionsMenu->Initialize();
   m_pChallengeMenu->SetPosition(l_Position, kNormal, false);
   m_pChallengeMenu->Initialize();
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
      if (m_pChallengeButton->GetPosition().x + m_pChallengeButton->GetBoundingBox().width/2 > GetResources()->GetWindow()->getSize().x / 2)
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
      //~ GetResources()->GetGameConfigData()->SetProperty("GameType", "Challenge");
      //~ GetResources()->SetActiveLevel("Challenge", true);
      //~ std::cout << "Challenge" << std::endl;

      m_pChallengeMenu->SetActive(true);
      m_pChallengeMenu->SetVelocityX(-1000, kNormal);
      SetVelocityX(-1000, kNormal);
   }
   else if (a_Message.m_From == m_pFreePlayButton->GetUniqueId())
   {
      m_pFreePlayMenu->SetActive(true);
      m_pFreePlayMenu->SetVelocityX(-1000, kNormal);
      SetVelocityX(-1000, kNormal);
   }
   else if (a_Message.m_From == m_pOptionsButton->GetUniqueId())
   {
      m_pOptionsMenu->SetActive(true);
      m_pOptionsMenu->SetVelocityX(-1000, kNormal);
      SetVelocityX(-1000, kNormal);
   }
   else if (a_Message.m_Key == "Menu Change" && a_Message.m_Value == "cMainMenu")
   {
      SetVelocityX(1000, kNormal);
   }
}
