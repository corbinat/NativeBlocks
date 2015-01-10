#include "cFinalMenu.h"
#include "MainMenu/cButton.h"

#include <iostream>

cFinalMenu::cFinalMenu(cResources* a_pResources)
   : cObject(a_pResources),
     m_Initialized(false),
     m_pPlayAgainButton(NULL),
     m_pMainMenuButton(NULL)
{
   SetType("FinalMenu");

   SetSolid(false);
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

   if (!m_Initialized)
   {
      // Receive messages for when a player loses. That's our time to shine
      sMessage l_Request;
      l_Request.m_From = GetResources()->GetMessageDispatcher()->AnyID();
      l_Request.m_Category = GetResources()->GetMessageDispatcher()->Any();
      l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
      l_Request.m_Value = "Player Lost";;

      std::function<void(sMessage)> l_MessageCallback =
         std::bind(&cFinalMenu::MessageReceived, this, std::placeholders::_1);

      GetResources()->GetMessageDispatcher()->RegisterForMessages(
         GetUniqueId(),
         l_MessageCallback,
         l_Request
         );

      m_Initialized = true;
   }

}

void cFinalMenu::Draw()
{
}

void cFinalMenu::MessageReceived(sMessage a_Message)
{
   if (a_Message.m_Value == "Player Lost")
   {
      std::cout << "Detecting player lost" << std::endl;
      _CreateMenu();
   }
   else if (m_pPlayAgainButton != NULL && a_Message.m_From == m_pPlayAgainButton->GetUniqueId())
   {
      std::cout << "Play Again pressed" << std::endl;
      GetResources()->SetActiveLevel("Level1", true);
   }
   else if (m_pMainMenuButton != NULL && a_Message.m_From == m_pMainMenuButton->GetUniqueId())
   {
      std::cout << "Main Menu button pressed" << std::endl;
      GetResources()->SetActiveLevel("MainMenu", true);
   }
}

void cFinalMenu::_CreateMenu()
{
   // If the buttons have already been created, don't do it again
   if (m_pPlayAgainButton != NULL)
   {
      return;
   }

   m_pPlayAgainButton = new cButton(GetResources());
   m_pPlayAgainButton->SetImage("Media/Final.ani", "PlayAgainButton");
   sf::Vector3<double> l_Position = GetPosition();
   l_Position.y = 0;
   m_pPlayAgainButton->SetPosition(l_Position, kNormal, false);

   m_pMainMenuButton = new cButton(GetResources());
   m_pMainMenuButton->SetImage("Media/Final.ani", "MainMenuButton");
   l_Position.y += m_pPlayAgainButton->GetBoundingBox().height + 2;
   m_pMainMenuButton->SetPosition(l_Position, kNormal, false);

   GetResources()->GetMessageDispatcher()->CancelMessages(GetUniqueId());

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
}
