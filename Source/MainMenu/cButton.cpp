#include "cButton.h"
#include <iostream>

cButton::cButton(cResources* a_pResources)
   : cObject(a_pResources),
     m_SpriteImage("ButtonLeft"),
     m_SpritePressedImage("ButtonLeftPressed")
{
   SetType("Button");
   SetSolid(true);
   LoadAnimations("Media/Title.ani");
   PlayAnimationLoop(m_SpriteImage);
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
         sf::Rect<uint32_t> l_PositionBox = GetBoundingBox();
         l_PositionBox.left = GetPosition().x;
         l_PositionBox.top = GetPosition().y;

         if (l_PositionBox.contains(i->mouseButton.x, i->mouseButton.y))
         {
            PlayAnimationLoop(m_SpritePressedImage);
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
         PlayAnimationLoop(m_SpriteImage);
      }
   }
}

void cButton::Step (uint32_t a_ElapsedMiliSec)
{

}

void cButton::Draw()
{

}

void cButton::SetSprite(std::string a_Sprite, std::string a_SpritePressed)
{
   m_SpriteImage = a_Sprite;
   m_SpritePressedImage = a_SpritePressed;
   PlayAnimationLoop(m_SpriteImage);
}
