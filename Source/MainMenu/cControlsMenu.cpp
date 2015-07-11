#include "cControlsMenu.h"
#include "cEventCapture.h"
#include "Common/Constants.h"
#include "Common/Widgets/cSelectionBox.h"
#include "Common/Widgets/cButton.h"

#include <iostream>

cControlsMenu::cControlsMenu(cResources* a_pResources)
   : cObject(a_pResources),
     m_ActionButtons(),
     m_pBackButton(NULL),
     m_PostBackMessage(false)
{
   SetType("cControlsMenu");

   SetSolid(false);

   std::shared_ptr<sf::Font> l_Font
      = GetResources()->LoadFont("Media/junegull.ttf");

   for (int i = 0; i < kActionCount; ++i)
   {
      std::string l_LabelString;
      std::string l_ActionName;

      if (i == static_cast<int>(kActionDOwn))
      {
         l_LabelString = "Down: ";
         l_ActionName =
            GetResources()->GetEventTranslator()->GetActionToEventString(g_kActionDown);
      }
      else if (i == static_cast<int>(kActionLeft))
      {
         l_LabelString = "Left: ";
         l_ActionName =
            GetResources()->GetEventTranslator()->GetActionToEventString(g_kActionLeft);
      }
      else if (i == static_cast<int>(kActionRight))
      {
         l_LabelString = "Right: ";
         l_ActionName =
            GetResources()->GetEventTranslator()->GetActionToEventString(g_kActionRight);
      }
      else if (i == static_cast<int>(kActionClockwise))
      {
         l_LabelString = "Turn Right: ";
         l_ActionName =
            GetResources()->GetEventTranslator()->GetActionToEventString(g_kActionRotateClockwise);
      }
      else if (i == static_cast<int>(kActionCounterClockwise))
      {
         l_LabelString = "Turn Left: ";
         l_ActionName =
            GetResources()->GetEventTranslator()->GetActionToEventString(g_kActionRotateCounterClockwise);
      }
      else if (i == static_cast<int>(kActionPause))
      {
         l_LabelString = "Pause: ";
         l_ActionName =
            GetResources()->GetEventTranslator()->GetActionToEventString(g_kActionPause);
      }

      std::pair<sf::Text, cButton*> l_NewAction;

      l_NewAction.first.setFont(*(l_Font.get()));
      l_NewAction.first.setString(l_LabelString);
      l_NewAction.first.setCharacterSize(20);
      l_NewAction.first.setColor(sf::Color::Black);

      l_NewAction.second = new cButton(GetResources());
      l_NewAction.second->SetImage("Media/Title.ani", "BlankMediumButton");
      l_NewAction.second->SetLabel(l_ActionName);

      AddChild(l_NewAction.second);

      m_ActionButtons.push_back(l_NewAction);
   }

   m_pBackButton = new cButton(GetResources());
   m_pBackButton->SetImage("Media/Title.ani", "BlankMediumButton");
   m_pBackButton->SetLabel("Save");

   AddChild(m_pBackButton);
}

cControlsMenu::~cControlsMenu()
{
   GetResources()->GetMessageDispatcher()->CancelMessages(GetUniqueId());
}

void cControlsMenu::Initialize()
{
   sf::Vector3<double> l_Position = GetPosition();
   l_Position.x += 110;

   for (auto i = m_ActionButtons.begin(); i != m_ActionButtons.end(); ++i)
   {
      i->first.setPosition(GetPosition().x, l_Position.y + 4);
      i->second->SetPosition(l_Position, kNormal, false);
      l_Position.y += i->second->GetBoundingBox().height + 5;
   }

   m_pBackButton->SetPosition(l_Position, kNormal, false);

   // Need to Receive messages when the Control buttons are pushed. Buttons emit
   // a "Widget" category, so we'll just look for all of those as a shortcut.
   // This is kind of fragile. Need to change buttons to just have a callback
   sMessage l_Request;
   l_Request.m_From = GetResources()->GetMessageDispatcher()->AnyID();
   l_Request.m_Category = "Widget";
   l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Value = GetResources()->GetMessageDispatcher()->Any();

   std::function<void(sMessage)> l_MessageCallback =
      std::bind(&cControlsMenu::MessageReceived, this, std::placeholders::_1);

   GetResources()->GetMessageDispatcher()->RegisterForMessages(
      GetUniqueId(),
      l_MessageCallback,
      l_Request
      );

   // Receive messages for when we need up update your buttons
   l_Request.m_From = GetResources()->GetMessageDispatcher()->AnyID();
   l_Request.m_Category = "Update";
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

      for (auto i = m_ActionButtons.begin(); i != m_ActionButtons.end(); ++i)
      {
         i->first.setPosition(GetPosition().x, l_Position.y + 4);
         l_Position.y += i->second->GetBoundingBox().height + 5;
      }
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
   }

   if (GetVelocity().x < 0)
   {
      if (m_ActionButtons.begin()->second->GetPosition().x < GetResources()->GetWindow()->getSize().x / 2)
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

void cControlsMenu::Draw()
{
   for (auto i = m_ActionButtons.begin(); i != m_ActionButtons.end(); ++i)
   {
      GetResources()->GetWindow()->draw(i->first);
   }
}

void cControlsMenu::MessageReceived(sMessage a_Message)
{
   cEventCapture * l_Capture = NULL;
   if (a_Message.m_From == m_ActionButtons[kActionDOwn].second->GetUniqueId())
   {
      l_Capture = new cEventCapture(g_kActionDown, "Down", GetResources());
      l_Capture->SetPosition(GetPosition(), kNormal, false);
   }
   else if (a_Message.m_From == m_ActionButtons[kActionLeft].second->GetUniqueId())
   {
      l_Capture = new cEventCapture(g_kActionLeft, "Left", GetResources());
      l_Capture->SetPosition(GetPosition(), kNormal, false);
   }
   else if (a_Message.m_From == m_ActionButtons[kActionRight].second->GetUniqueId())
   {
      l_Capture = new cEventCapture(g_kActionRight, "Right", GetResources());
      l_Capture->SetPosition(GetPosition(), kNormal, false);
   }
   else if (a_Message.m_From == m_ActionButtons[kActionClockwise].second->GetUniqueId())
   {
      l_Capture = new cEventCapture(g_kActionRotateClockwise, "Turn Right", GetResources());
      l_Capture->SetPosition(GetPosition(), kNormal, false);
   }
   else if (a_Message.m_From == m_ActionButtons[kActionCounterClockwise].second->GetUniqueId())
   {
      l_Capture = new cEventCapture(g_kActionRotateCounterClockwise, "Turn Left", GetResources());
      l_Capture->SetPosition(GetPosition(), kNormal, false);
   }
   else if (a_Message.m_From == m_ActionButtons[kActionPause].second->GetUniqueId())
   {
      l_Capture = new cEventCapture(g_kActionPause, "Pause", GetResources());
      l_Capture->SetPosition(GetPosition(), kNormal, false);
   }
   else if (a_Message.m_From == m_pBackButton->GetUniqueId())
   {
      SetVelocityX(1000, kNormal);
      m_PostBackMessage = true;
   }
   else if (a_Message.m_Category == "Update")
   {
      _UpdateButtons();
   }

   if (l_Capture != NULL)
   {
      sf::Vector3<double> l_Position = GetPosition();
      l_Position.y -= 50;
      l_Capture->SetPosition(l_Position, kNormal, false);
      l_Capture->Initialize();
   }
}

void cControlsMenu::_UpdateButtons()
{
   for (auto i = 0; i < m_ActionButtons.size(); ++i)
   {
      std::string l_ActionName;

      if (i == static_cast<int>(kActionDOwn))
      {
         l_ActionName =
            GetResources()->GetEventTranslator()->GetActionToEventString(g_kActionDown);
      }
      else if (i == static_cast<int>(kActionLeft))
      {
         l_ActionName =
            GetResources()->GetEventTranslator()->GetActionToEventString(g_kActionLeft);
      }
      else if (i == static_cast<int>(kActionRight))
      {
         l_ActionName =
            GetResources()->GetEventTranslator()->GetActionToEventString(g_kActionRight);
      }
      else if (i == static_cast<int>(kActionClockwise))
      {
         l_ActionName =
            GetResources()->GetEventTranslator()->GetActionToEventString(g_kActionRotateClockwise);
      }
      else if (i == static_cast<int>(kActionCounterClockwise))
      {
         l_ActionName =
            GetResources()->GetEventTranslator()->GetActionToEventString(g_kActionRotateCounterClockwise);
      }
      else if (i == static_cast<int>(kActionPause))
      {
         l_ActionName =
            GetResources()->GetEventTranslator()->GetActionToEventString(g_kActionPause);
      }

      m_ActionButtons[i].second->SetLabel(l_ActionName);
   }
}
