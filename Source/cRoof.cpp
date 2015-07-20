#include "cRoof.h"

#include "cPortrait.h"

cRoof::cRoof(cResources* a_pResources, std::string a_Label)
   : cObject(a_pResources),
     m_PlayerName(),
     m_pPortrait(NULL)
{
   SetType("Roof");
   SetSolid(false);
   SetCollidable(false);
   LoadAnimations("Media/Floor.ani");
   PlayAnimationLoop("Roof");
   SetDepth(-2);

   std::shared_ptr<sf::Font> l_Font
      = GetResources()->LoadFont("Media/junegull.ttf");
   m_PlayerName.setFont(*(l_Font.get()));
   m_PlayerName.setString(a_Label);
   m_PlayerName.setCharacterSize(20);
   m_PlayerName.setColor(sf::Color::Black);

   m_pPortrait = new cPortrait(a_pResources, a_Label);
}

cRoof::~cRoof()
{
}

void cRoof::Initialize()
{
   m_PlayerName.setPosition(GetPosition().x + 32 + 4, GetPosition().y + 32);

   sf::Vector3<double> l_Position = GetPosition();
   l_Position.x += 32 * 2;

   // The portrait is 58 pixels tall so it will be 6 pixels short of touching
   // the bottom of the roof at 32 * 2. Frame starts 13 pixels up, so 13 - 6 = 7
   l_Position.y += 32 * 2 - 7;

   m_pPortrait->SetPosition(l_Position, kNormal, false);
}

void cRoof::Event(std::list<sf::Event> * a_pEventList)
{
}

void cRoof::Step (uint32_t a_ElapsedMiliSec)
{

}
void cRoof::Draw(const sf::Vector2<float> & a_rkInterpolationOffset)
{
   GetResources()->GetWindow()->draw(m_PlayerName);
}

void cRoof::Collision(cObject* a_pOther)
{
}
