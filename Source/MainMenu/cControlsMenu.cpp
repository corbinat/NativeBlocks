#include "cControlsMenu.h"
#include "cEventCapture.h"
#include "Common/Constants.h"
#include "Common/Widgets/cSelectionBox.h"
#include "Common/Widgets/cButton.h"

#include <iostream>

cControlsMenu::cControlsMenu(cResources* a_pResources)
   : cObject(a_pResources),
     m_DownLabel(),
     m_pDownButton(NULL),
     m_pBackButton(NULL),
     m_PostBackMessage(false)
{
   SetType("cControlsMenu");

   SetSolid(false);

   std::shared_ptr<sf::Font> l_Font
      = GetResources()->LoadFont("Media/junegull.ttf");

   m_DownLabel.setFont(*(l_Font.get()));
   m_DownLabel.setString("Down: ");
   m_DownLabel.setCharacterSize(20);
   m_DownLabel.setColor(sf::Color::Black);

   m_pDownButton = new cButton(GetResources());
   m_pDownButton->SetImage("Media/Title.ani", "BlankMediumButton");
   std::string l_DownAction =
      GetResources()->GetEventTranslator()->GetActionToEventString(g_kActionDown);
   m_pDownButton->SetLabel(l_DownAction);

   m_pBackButton = new cButton(GetResources());
   m_pBackButton->SetImage("Media/Title.ani", "BlankMediumButton");
   m_pBackButton->SetLabel("Cancel");

   AddChild(m_pDownButton);
   AddChild(m_pBackButton);
}

cControlsMenu::~cControlsMenu()
{
   GetResources()->GetMessageDispatcher()->CancelMessages(GetUniqueId());
}

void cControlsMenu::Initialize()
{
   sf::Vector3<double> l_Position = GetPosition();
   m_DownLabel.setPosition(GetPosition().x, l_Position.y + 4);
   l_Position.x += 90;
   m_pDownButton->SetPosition(l_Position, kNormal, false);
   l_Position.y += m_pDownButton->GetBoundingBox().height + 5;
   m_pBackButton->SetPosition(l_Position, kNormal, false);

   // Receive messages when The Controls button is pushed.
   sMessage l_Request;
   l_Request.m_From = m_pDownButton->GetUniqueId();
   l_Request.m_Category = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Value = GetResources()->GetMessageDispatcher()->Any();
//~
   std::function<void(sMessage)> l_MessageCallback =
      std::bind(&cControlsMenu::MessageReceived, this, std::placeholders::_1);
//~
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

void cControlsMenu::Collision(cObject* a_pOther)
{

}

void cControlsMenu::Event(std::list<sf::Event> * a_pEventList)
{
}

void cControlsMenu::Step (uint32_t a_ElapsedMiliSec)
{
   if (GetPosition() != GetPreviousPosition())
   {
      sf::Vector3<double> l_Position = GetPosition();
      m_DownLabel.setPosition(GetPosition().x, l_Position.y + 4);
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
      l_Message.m_Value = "cOptionsMenu";
      GetResources()->GetMessageDispatcher()->PostMessage(l_Message);
      m_PostBackMessage = false;

      std::cout << "Posting back button" << std::endl;
   }
}

void cControlsMenu::Draw()
{
   GetResources()->GetWindow()->draw(m_DownLabel);
}

void cControlsMenu::MessageReceived(sMessage a_Message)
{
   if (a_Message.m_From == m_pDownButton->GetUniqueId())
   {
      cEventCapture * l_Capture = new cEventCapture(g_kActionDown, GetResources());
   }
   else if (a_Message.m_From == m_pBackButton->GetUniqueId())
   {
      m_PostBackMessage = true;
   }
}
