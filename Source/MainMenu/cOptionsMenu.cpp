#include "cOptionsMenu.h"
#include "cControlsMenu.h"
#include "Common/Widgets/cSelectionBox.h"
#include "Common/Widgets/cButton.h"
#include "Common/Widgets/cSlider.h"

#include <iostream>

cOptionsMenu::cOptionsMenu(cResources* a_pResources)
   : cObject(a_pResources),
     m_Active(false),
     m_SoundLabel(),
     m_MusicLabel(),
     m_pSoundSlider(NULL),
     m_pMusicSlider(NULL),
     m_pKeyMapButton(NULL),
     m_pControlsMenu(NULL),
     m_pBackButton(NULL),
     m_PostBackMessage(false)
{
   SetType("cOptionsMenu");

   SetSolid(false);

   std::shared_ptr<sf::Font> l_Font
      = GetResources()->LoadFont("Media/junegull.ttf");

   m_SoundLabel.setFont(*(l_Font.get()));
   m_SoundLabel.setString("Sound Volume: ");
   m_SoundLabel.setCharacterSize(20);
   m_SoundLabel.setColor(sf::Color::Black);

   m_MusicLabel.setFont(*(l_Font.get()));
   m_MusicLabel.setString("Music   Volume: ");
   m_MusicLabel.setCharacterSize(20);
   m_MusicLabel.setColor(sf::Color::Black);

   m_pSoundSlider = new cSlider(GetResources());
   m_pMusicSlider = new cSlider(GetResources());

   m_pKeyMapButton = new cButton(GetResources());
   m_pKeyMapButton->SetImage("Media/Title.ani", "BlankMediumButton");
   m_pKeyMapButton->SetLabel("Controls");

   m_pControlsMenu = new cControlsMenu(GetResources());

   m_pBackButton = new cButton(GetResources());
   m_pBackButton->SetImage("Media/Title.ani", "BlankMediumButton");
   m_pBackButton->SetLabel("Back");

   AddChild(m_pSoundSlider);
   AddChild(m_pMusicSlider);
   AddChild(m_pKeyMapButton);
   AddChild(m_pBackButton);
}

cOptionsMenu::~cOptionsMenu()
{
   GetResources()->GetMessageDispatcher()->CancelMessages(GetUniqueId());
}

void cOptionsMenu::SetActive(bool a_Active)
{
   m_Active = a_Active;
}

// These functions are overloaded from cObject
void cOptionsMenu::Initialize()
{
   sf::Vector3<double> l_Position = GetPosition();
   m_SoundLabel.setPosition(l_Position.x, l_Position.y );
   l_Position.x += 150;
   m_pSoundSlider->SetPosition(l_Position, kNormal, false);
   l_Position.y += m_pSoundSlider->GetBoundingBox().height + 10;

   m_MusicLabel.setPosition(GetPosition().x, l_Position.y);
   m_pMusicSlider->SetPosition(l_Position, kNormal, false);

   l_Position.y += m_pMusicSlider->GetBoundingBox().height + 15;
   m_pKeyMapButton->SetPosition(l_Position, kNormal, false);
   l_Position.y += m_pKeyMapButton->GetBoundingBox().height + 5;
   m_pBackButton->SetPosition(l_Position, kNormal, false);

   // Initialize widgets
   m_pSoundSlider->Initialize();
   m_pMusicSlider->Initialize();

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

   // Receive messages from the sound slider
   l_Request.m_From = m_pSoundSlider->GetUniqueId();
   l_Request.m_Category = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Value = GetResources()->GetMessageDispatcher()->Any();

   GetResources()->GetMessageDispatcher()->RegisterForMessages(
      GetUniqueId(),
      l_MessageCallback,
      l_Request
      );

   // Receive messages from the music slider
   l_Request.m_From = m_pMusicSlider->GetUniqueId();
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

   // Initialize values for the sliders
   m_pMusicSlider->SetValue(GetResources()->GetBackGroundMusic()->getVolume());
   m_pSoundSlider->SetValue(GetResources()->GetSoundVolume());
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

   if (GetPosition() != GetPreviousPosition())
   {
      sf::Vector3<double> l_Position = m_pSoundSlider->GetPosition();
      l_Position.y -= m_SoundLabel.getLocalBounds().top;
      m_SoundLabel.setPosition(GetPosition().x, l_Position.y);

      l_Position = m_pMusicSlider->GetPosition();
      l_Position.y -= m_MusicLabel.getLocalBounds().top;
      m_MusicLabel.setPosition(GetPosition().x, l_Position.y);
   }

   double l_CenterPoint =
      m_MusicLabel.getLocalBounds().left +
      m_MusicLabel.getLocalBounds().width +
      m_pMusicSlider->GetBoundingBox().width;
   l_CenterPoint = l_CenterPoint / 2;
   l_CenterPoint += m_MusicLabel.getPosition().x;

   if (GetVelocity().x < 0)
   {
      if (!m_Active)
      {
         //~ if (GetPosition().x < (0 - static_cast<int32_t>(m_pKeyMapButton->GetBoundingBox().width)))
         if (m_pKeyMapButton->GetPosition().x + m_pKeyMapButton->GetBoundingBox().width < 0 )
         {
            SetVelocityX(0, kNormal);
            std::cout << "POW1" << std::endl;
         }
      }
      else
      {
         if (l_CenterPoint < GetResources()->GetWindow()->getSize().x / 2)
         {
            SetVelocityX(0, kNormal);
            std::cout << "POW2" << std::endl;
         }
      }
   }
   else if (GetVelocity().x > 0)
   {
      if (!m_Active)
      {
         if (GetPosition().x > GetResources()->GetWindow()->getSize().x)
         {
            SetVelocityX(0, kNormal);
            std::cout << "POW3" << std::endl;
         }
      }
      else
      {
         if (l_CenterPoint > GetResources()->GetWindow()->getSize().x / 2)
         {
            SetVelocityX(0, kNormal);
         }
      }
   }
}

void cOptionsMenu::Draw()
{
   GetResources()->GetWindow()->draw(m_SoundLabel);
   GetResources()->GetWindow()->draw(m_MusicLabel);
}

void cOptionsMenu::MessageReceived(sMessage a_Message)
{
   if (a_Message.m_From == m_pKeyMapButton->GetUniqueId())
   {
      m_pControlsMenu->SetVelocityX(-1000, kNormal);
      SetVelocityX(-1000, kNormal);
      m_Active = false;
   }
   else if (a_Message.m_From == m_pBackButton->GetUniqueId())
   {
      SetVelocityX(1000, kNormal);
      m_PostBackMessage = true;
      m_Active = false;
   }
   else if (a_Message.m_From == m_pSoundSlider->GetUniqueId())
   {
      double l_Volume = std::stod(a_Message.m_Value);
      GetResources()->SetSoundVolume(l_Volume);
   }
   else if (a_Message.m_From == m_pMusicSlider->GetUniqueId())
   {
      double l_Volume = std::stod(a_Message.m_Value);
      GetResources()->GetBackGroundMusic()->setVolume(l_Volume);
   }
   else if (a_Message.m_Key == "Menu Change" && a_Message.m_Value == "cOptionsMenu")
   {
      SetVelocityX(1000, kNormal);
      m_Active = true;
   }
}
