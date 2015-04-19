#include "cOptionsMenu.h"
#include "cControlsMenu.h"
#include "Common/Widgets/cSelectionBox.h"
#include "Common/Widgets/cButton.h"

#include <iostream>

cOptionsMenu::cOptionsMenu(cResources* a_pResources)
   : cObject(a_pResources),
     m_pKeyMapButton(NULL),
     m_pControlsMenu(NULL),
     m_pBackButton(NULL),
     m_PostBackMessage(false)
{
   SetType("cOptionsMenu");

   SetSolid(false);

   std::shared_ptr<sf::Font> l_Font
      = GetResources()->LoadFont("Media/junegull.ttf");

   m_pKeyMapButton = new cButton(GetResources());
   m_pKeyMapButton->SetImage("Media/Title.ani", "BlankMediumButton");
   m_pKeyMapButton->SetLabel("Controls");

   m_pControlsMenu = new cControlsMenu(GetResources());

   m_pBackButton = new cButton(GetResources());
   m_pBackButton->SetImage("Media/Title.ani", "BlankSmallButton");
   m_pBackButton->SetLabel("Back");

   AddChild(m_pKeyMapButton);
   AddChild(m_pBackButton);
}

cOptionsMenu::~cOptionsMenu()
{
   GetResources()->GetMessageDispatcher()->CancelMessages(GetUniqueId());
}

// These functions are overloaded from cObject
void cOptionsMenu::Initialize()
{
   sf::Vector3<double> l_Position = GetPosition();
   m_pKeyMapButton->SetPosition(l_Position, kNormal, false);
   l_Position.y += m_pKeyMapButton->GetBoundingBox().height + 5;
   m_pBackButton->SetPosition(l_Position, kNormal, false);

   // Receive messages when The Controls button is pushed.
   sMessage l_Request;
   l_Request.m_From = m_pKeyMapButton->GetUniqueId();
   l_Request.m_Category = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Value = GetResources()->GetMessageDispatcher()->Any();

   std::function<void(sMessage)> l_MessageCallback =
      std::bind(&cOptionsMenu::MessageReceived, this, std::placeholders::_1);

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

   // Receive messages for when this menu should be active
   l_Request.m_From = GetResources()->GetMessageDispatcher()->AnyID();
   l_Request.m_Category = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Key = "Menu Change";
   l_Request.m_Value = "cOptionsMenu";

   GetResources()->GetMessageDispatcher()->RegisterForMessages(
      GetUniqueId(),
      l_MessageCallback,
      l_Request
      );

   // Set up the other menus
   l_Position = GetPosition();
   l_Position.x = GetResources()->GetWindow()->getSize().x;
   //~ l_Position.x = GetResources()->GetWindow()->getSize().x - 200;
   m_pControlsMenu->SetPosition(l_Position, kNormal, false);
   m_pControlsMenu->Initialize();
}

void cOptionsMenu::Collision(cObject* a_pOther)
{

}

void cOptionsMenu::Event(std::list<sf::Event> * a_pEventList)
{
}

void cOptionsMenu::Step (uint32_t a_ElapsedMiliSec)
{
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
      if (GetPosition().x < (0 - static_cast<int32_t>(m_pKeyMapButton->GetBoundingBox().width)))
      {
         SetVelocityX(0, kNormal);
      }
   }
   else if (GetVelocity().x > 0)
   {
      if (m_pControlsMenu->GetPosition().x >= 800)
      {
         SetVelocityX(0, kNormal);

         // The main menu moved back in place. Remove children menus
         RemoveChild(m_pControlsMenu);
      }
   }
}

void cOptionsMenu::Draw()
{

}

void cOptionsMenu::MessageReceived(sMessage a_Message)
{
   if (a_Message.m_From == m_pKeyMapButton->GetUniqueId())
   {
      AddChild(m_pControlsMenu);
      SetVelocityX(-1000, kNormal);
   }
   else if (a_Message.m_From == m_pBackButton->GetUniqueId())
   {
      m_PostBackMessage = true;
   }
   else if (a_Message.m_Key == "Menu Change" && a_Message.m_Value == "cOptionsMenu")
   {
      SetVelocityX(1000, kNormal);
      std::cout << "Moving back" << std::endl;
   }
}
