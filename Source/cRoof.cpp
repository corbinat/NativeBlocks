#include "cRoof.h"

cRoof::cRoof(cResources* a_pResources, std::string a_Label)
   : cObject(a_pResources),
     m_PlayerName()
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
   m_PlayerName.setColor(sf::Color::White);
}

cRoof::~cRoof()
{
}

void cRoof::Initialize()
{
   m_PlayerName.setPosition(GetPosition().x + 32 + 4, GetPosition().y + 32 * 3);
}

void cRoof::Event(std::list<sf::Event> * a_pEventList)
{
}

void cRoof::Step (uint32_t a_ElapsedMiliSec)
{

}
void cRoof::Draw()
{
   GetResources()->GetWindow()->draw(m_PlayerName);
}

void cRoof::Collision(cObject* a_pOther)
{
}
