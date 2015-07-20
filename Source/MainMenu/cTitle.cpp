#include "cTitle.h"

#include <iostream>

cTitle::cTitle(cResources* a_pResources)
   : cObject(a_pResources),
     m_DemoString()
{
   SetType("TitleBox");
   SetSolid(true);
   LoadAnimations("Media/Title.ani");
   PlayAnimationLoop("TitleBanner");

   // REMOVE THIS WHEN GAME IS COMPLETE
   std::shared_ptr<sf::Font> l_Font
      = GetResources()->LoadFont("Media/junegull.ttf");
   m_DemoString.setFont(*(l_Font.get()));
   m_DemoString.setCharacterSize(20);
   m_DemoString.setColor(sf::Color::Black);
   m_DemoString.setString("DEMO BUILD");
}

cTitle::~cTitle()
{
}

void cTitle::Initialize()
{
   sf::Vector3<double> l_Position = GetPosition();
   l_Position.y += GetBoundingBox().height;
   m_DemoString.setPosition(l_Position.x, l_Position.y);
}

// These functions are overloaded from cObject
void cTitle::Collision(cObject* a_pOther)
{

}

void cTitle::Event(std::list<sf::Event> * a_pEventList)
{
}

void cTitle::Step (uint32_t a_ElapsedMiliSec)
{
}

void cTitle::Draw(const sf::Vector2<float> & a_rkInterpolationOffset)
{
   GetResources()->GetWindow()->draw(m_DemoString);
}
