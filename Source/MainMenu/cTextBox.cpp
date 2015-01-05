#include "cTextBox.h"
#include <iostream>

cTextBox::cTextBox(cResources* a_pResources)
   : cObject(a_pResources),
     m_String("Test"),
     m_Data(),
     m_BackGround(),
     m_Initialized(false)
{
   SetType("TextBox");

   SetSolid(false);

   std::shared_ptr<sf::Font> l_Font
      = GetResources()->LoadFont("Media/UASQUARE.TTF");
   m_Data.setFont(*(l_Font.get()));
   m_Data.setString(m_String);
   m_Data.setCharacterSize(12);
   m_Data.setColor(sf::Color::Black);

   m_BackGround.setSize(sf::Vector2f(100, 18));
   SetBoundingBox(sf::Rect<uint32_t>(0,0,100,18));
   m_BackGround.setOutlineThickness(0);
   m_BackGround.setFillColor(sf::Color(204, 204, 204));

}

cTextBox::~cTextBox()
{

}

// These functions are overloaded from cObject
void cTextBox::Collision(cObject* a_pOther)
{

}

void cTextBox::Event(std::list<sf::Event> * a_pEventList)
{
}

void cTextBox::Step (uint32_t a_ElapsedMiliSec)
{
   if (!m_Initialized)
   {
      m_Data.setPosition(GetPosition().x + 4, GetPosition().y + 4);
      m_BackGround.setPosition(GetPosition().x, GetPosition().y);
   }
}

void cTextBox::Draw()
{
   //std::cout << "Drawing text" << std::endl;
   GetResources()->GetWindow()->draw(m_BackGround);
   GetResources()->GetWindow()->draw(m_Data);
}

void cTextBox::ReplaceString(std::string a_String)
{
   std::cout << "Replacing string with " << a_String << std::endl;
   m_String = a_String;
   std::cout << "OK" << std::endl;
   m_Data.setString(m_String);
   std::cout << "Done" << std::endl;
}
