#include "cCreditsString.h"
#include "cSfmlLogo.h"

#include <iostream>

cCreditsString::cCreditsString(cResources* a_pResources)
   : cObject(a_pResources),
     m_CreditText(),
     m_CreditText2(),
     m_ContinueString(),
     m_pSfmlLogo(NULL)
{
   SetType("CreditsString");
   SetSolid(false);
   SetCollidable(false);

   std::shared_ptr<sf::Font> l_Font
      = GetResources()->LoadFont("Media/junegull.ttf");

   // Set up the strings
   m_CreditText.setFont(*(l_Font.get()));
   m_CreditText.setString("A game by Corbin A.");
   m_CreditText.setCharacterSize(30);
   m_CreditText.setColor(sf::Color::Black);

   m_CreditText2.setFont(*(l_Font.get()));
   m_CreditText2.setString("Created with");
   m_CreditText2.setCharacterSize(30);
   m_CreditText2.setColor(sf::Color::Black);

   // Set up the continue message
   m_ContinueString.setFont(*(l_Font.get()));
   m_ContinueString.setCharacterSize(18);
   m_ContinueString.setColor(sf::Color::White);
   m_ContinueString.setString("Press any button to continue");

   m_pSfmlLogo = new cSfmlLogo(GetResources());

}

cCreditsString::~cCreditsString()
{

}

// These functions are overloaded from cObject

void cCreditsString::Initialize()
{
   sf::Vector3<double> l_Position = GetPosition();
   l_Position.x += (GetBoundingBox().width/2) - m_CreditText.getLocalBounds().width / 2;
   l_Position.x = static_cast<int32_t>(l_Position.x);
   m_CreditText.setPosition(l_Position.x, l_Position.y);

   l_Position = GetPosition();
   l_Position.x += (GetBoundingBox().width/2) - m_CreditText2.getLocalBounds().width / 2;
   l_Position.x = static_cast<int32_t>(l_Position.x);
   l_Position.y +=
      m_CreditText.getLocalBounds().height
      + m_CreditText.getLocalBounds().top
      + 100;
   m_CreditText2.setPosition(l_Position.x, l_Position.y);

   l_Position.x = GetPosition().x;
   l_Position.x += (GetBoundingBox().width/2) - m_pSfmlLogo->GetBoundingBox().width / 2;
   l_Position.y +=
      m_CreditText2.getLocalBounds().height
      + m_CreditText2.getLocalBounds().top
      + 10;
   m_pSfmlLogo->SetPosition(l_Position, kNormal, false);
   m_pSfmlLogo->Initialize();

   // Set the position relative to the camera
   float l_X = GetResources()->GetWindow()->getSize().x - m_ContinueString.getLocalBounds().width - 10;
   float l_Y = GetResources()->GetWindow()->getSize().y - m_ContinueString.getCharacterSize() - 10;
   m_ContinueString.setPosition(l_X, l_Y);
}

void cCreditsString::Collision(cObject* a_pOther)
{

}

void cCreditsString::Event(std::list<sf::Event> * a_pEventList)
{
   for (std::list<sf::Event>::iterator i = a_pEventList->begin();
      i != a_pEventList->end();
      ++i
      )
   {
      switch((*i).type)
      {
         case sf::Event::KeyPressed:
         case sf::Event::JoystickButtonPressed:
         {
            GetResources()->SetActiveLevel("MainMenu", true);
            break;
         }
         default:
         {
            break;
         }
      }
   }
}

void cCreditsString::Step (uint32_t a_ElapsedMiliSec)
{
}

void cCreditsString::Draw(const sf::Vector2<float> & a_rkInterpolationOffset)
{
   GetResources()->GetWindow()->draw(m_CreditText);
   GetResources()->GetWindow()->draw(m_CreditText2);
   GetResources()->GetWindow()->draw(m_ContinueString);
}
