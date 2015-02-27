#include "cButton.h"
#include <iostream>

cButton::cButton(cResources* a_pResources)
   : cObject(a_pResources),
     m_SpriteImage("Media/Title.ani", "ButtonLeft"),
     m_SpritePressedImage("Media/Title.ani", "ButtonLeftPressed")
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
            l_Message.m_Category = "Button";
            l_Message.m_Key = "Pressed";
            l_Message.m_Value = "True";
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

}

void cButton::Draw()
{

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
