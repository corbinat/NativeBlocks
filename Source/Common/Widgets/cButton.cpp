#include "cButton.h"
#include <iostream>

cButton::cButton(cResources* a_pResources)
   : cObject(a_pResources),
     m_SpriteImage("Media/Title.ani", "ButtonLeft"),
     m_SpritePressedImage("Media/Title.ani", "ButtonLeftPressed"),
     m_Label()
{
   SetType("Button");
   SetSolid(true);

   // Set default animations
   LoadAnimations(m_SpriteImage.first);
   PlayAnimationLoop(m_SpriteImage.second);
}

cButton::~cButton()
{

}

// These functions are overloaded from cObject
void cButton::Collision(cObject* a_pOther)
{

}

void cButton::Event(std::list<sf::Event> * a_pEventList)
{
   for (std::list<sf::Event>::iterator i = a_pEventList->begin();
      i != a_pEventList->end();
      ++i
      )
   {
      if (i->type == sf::Event::MouseButtonPressed)
      {
         // See if the mouse press was on us
         sf::Rect<int32_t> l_PositionBox = GetBoundingBox();
         l_PositionBox.left += GetPosition().x;
         l_PositionBox.top += GetPosition().y;

         if (l_PositionBox.contains(i->mouseButton.x, i->mouseButton.y))
         {
            PlayAnimationLoop(m_SpritePressedImage.second);
            PlaySound("Media/Sounds/Click1.wav");
            sMessage l_Message;
            l_Message.m_From = GetUniqueId();
            l_Message.m_Category = "Widget";
            l_Message.m_Key = "Button";
            l_Message.m_Value = "Pressed";
            GetResources()->GetMessageDispatcher()->PostMessage(l_Message);
         }
      }
      else if (i->type == sf::Event::MouseButtonReleased)
      {
         PlayAnimationLoop(m_SpriteImage.second);
      }
   }
}

void cButton::Step(uint32_t a_ElapsedMiliSec)
{
   if (m_Label.getString() != "")
   {
      m_Label.setPosition(
         static_cast<int32_t>(GetPosition().x + GetBoundingBox().left + GetBoundingBox().width / 2.0 - m_Label.getLocalBounds().width / 2.0),
          static_cast<int32_t>(GetPosition().y + GetBoundingBox().height / 2.0 - (m_Label.getLocalBounds().height + m_Label.getLocalBounds().top + 10) / 2.0)
         );
   }
}

void cButton::Draw()
{
   GetResources()->GetWindow()->draw(m_Label);
}

void cButton::SetImage(std::string a_AniFile, std::string a_Sprite)
{
   m_SpriteImage.first = a_AniFile;
   m_SpriteImage.second = a_Sprite;
   m_SpritePressedImage.first = a_AniFile;
   m_SpritePressedImage.second = a_Sprite;

   LoadAnimations(m_SpriteImage.first);
   PlayAnimationLoop(m_SpriteImage.second);
}

void cButton::SetPressedImage(std::string a_AniFile, std::string a_Sprite)
{
   m_SpritePressedImage.first = a_AniFile;
   m_SpritePressedImage.second = a_Sprite;

   LoadAnimations(m_SpritePressedImage.first);
}

void cButton::SetLabel(std::string a_String, uint32_t a_Size)
{
   std::shared_ptr<sf::Font> l_Font
      = GetResources()->LoadFont("Media/junegull.ttf");
   m_Label.setFont(*(l_Font.get()));
   m_Label.setCharacterSize(a_Size);
   m_Label.setColor(sf::Color::Black);
   m_Label.setString(a_String);
}
