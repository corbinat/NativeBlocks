#include "cOptionsMenu.h"
#include "Common/Widgets/cSelectionBox.h"
#include "Common/Widgets/cButton.h"

#include "../cAiPersonality.h"

#include <iostream>

cOptionsMenu::cOptionsMenu(cResources* a_pResources)
   : cObject(a_pResources),
     m_pKeyMapButton(NULL),
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
}

void cOptionsMenu::Draw()
{

}

void cOptionsMenu::MessageReceived(sMessage a_Message)
{
   if (a_Message.m_From == m_pKeyMapButton->GetUniqueId())
   {

   }
   else if (a_Message.m_From == m_pBackButton->GetUniqueId())
   {
      m_PostBackMessage = true;
   }
}
