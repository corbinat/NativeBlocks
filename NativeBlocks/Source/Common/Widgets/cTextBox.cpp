#include "cTextBox.h"
#include <iostream>

cTextBox::cTextBox(cResources* a_pResources)
   : cObject(a_pResources),
     m_String("Test"),
     m_Data()
{
   SetType("TextBox");

   SetSolid(false);

   std::shared_ptr<sf::Font> l_Font
      = GetResources()->LoadFont("Media/junegull.ttf");
   m_Data.setFont(*(l_Font.get()));
   m_Data.setString(m_String);
   m_Data.setCharacterSize(20);
   m_Data.setColor(sf::Color::Black);

   LoadAnimations("Media/Title.ani");
   PlayAnimationLoop("TextBox");
}

cTextBox::~cTextBox()
{

}

// These functions are overloaded from cObject
void cTextBox::Initialize()
{
   m_Data.setPosition(GetPosition().x + 4, GetPosition().y + 4);
}

void cTextBox::Collision(cObject* a_pOther)
{

}

void cTextBox::Event(std::list<sf::Event> * a_pEventList)
{
}

void cTextBox::Step (uint32_t a_ElapsedMiliSec)
{
   if (GetPosition() != GetPreviousPosition())
   {
      m_Data.setPosition(GetPosition().x + 4, GetPosition().y + 4);
   }
}

void cTextBox::Draw(const sf::Vector2<float> & a_rkInterpolationOffset)
{
   sf::Vector2<float> l_OldPosition = m_Data.getPosition();
   m_Data.setPosition(l_OldPosition + a_rkInterpolationOffset);
   GetResources()->GetWindow()->draw(m_Data);
   m_Data.setPosition(l_OldPosition);
}

void cTextBox::ReplaceString(std::string a_String)
{
   m_String = a_String;
   m_Data.setString(m_String);
}
