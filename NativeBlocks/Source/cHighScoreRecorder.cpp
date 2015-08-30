#include "cHighScoreRecorder.h"

cHighScoreRecorder::cHighScoreRecorder(cResources* a_pResources)
   : cObject(a_pResources),
     m_Description(),
     m_Name()
{
   SetType("cHighScoreRecorder");
   SetSolid(false);
   SetCollidable(false);
   SetDepth(-10);
   LoadAnimations("Media/HighScoreBg.ani");
   PlayAnimationLoop("HighScoreRecorderBg");

   cVFileNode l_HighScores;
   l_HighScores.LoadFile("Config/HighScores.cfg");

   std::shared_ptr<sf::Font> l_Font
      = GetResources()->LoadFont("Media/junegull.ttf");

   // Set up the title
   m_Description.setFont(*(l_Font.get()));
   m_Description.setString("Enter Name For High Score List!");
   m_Description.setCharacterSize(30);
   m_Description.setColor(sf::Color::Black);

   m_Name.setFont(*(l_Font.get()));
   m_Name.setString("Name: _");
   m_Name.setCharacterSize(30);
   m_Name.setColor(sf::Color::Black);

   // Center on screen
   sf::Vector3<double> l_Position;
   l_Position.x = GetResources()->GetWindow()->getSize().x / 2 - GetBoundingBox().width / 2;
   l_Position.y += 32 * 3;
   SetPosition(l_Position, kNormal, false);
   Initialize();
}

cHighScoreRecorder::~cHighScoreRecorder()
{
}

void cHighScoreRecorder::Initialize()
{
   sf::Vector3<double> l_Position = GetPosition();
   l_Position.x += (GetBoundingBox().width/2) - m_Description.getLocalBounds().width / 2;
   m_Description.setPosition(l_Position.x, l_Position.y);

   l_Position = GetPosition();
   l_Position.y += m_Description.getCharacterSize() * 2;
   l_Position.x += 64;
   m_Name.setPosition(l_Position.x, l_Position.y);
}

void cHighScoreRecorder::Event(std::list<sf::Event> * a_pEventList)
{
   bool l_Continue = false;
   for (std::list<sf::Event>::iterator i = a_pEventList->begin();
      i != a_pEventList->end();
      ++i
      )
   {
      switch((*i).type)
      {
         case sf::Event::TextEntered:
         {
            // Using sf::String because it is a uint32_t based string. The text
            // event produces utf-32 values.
            sf::String l_Name;
            l_Name = m_Name.getString();

            // Remove the trailing underscore
            if (l_Name.getSize() > 0 && l_Name[l_Name.getSize()-1] == sf::String("_"))
            {
               l_Name.erase(l_Name.getSize() - 1, 1);
            }

            if ((*i).text.unicode == '\b')
            {
               if (l_Name.getSize() > 6)
               {
                  l_Name.erase(l_Name.getSize() - 1, 1);
               }
            }
            else if (l_Name.getSize() < 7 + 6)
            {
               l_Name += (*i).text.unicode;
            }

            l_Name += "_";
            m_Name.setString(l_Name);
            break;
         }
         case sf::Event::KeyPressed:
         case sf::Event::MouseButtonPressed:
         case sf::Event::JoystickButtonPressed:
         {
            if ((*i).type == sf::Event::KeyPressed)
            {
               if ((*i).key.code ==  sf::Keyboard::Return)
               {
                  l_Continue = true;
               }
            }

            //~ if (  GetResources()->GetEventTranslator()->Compare(*i, g_kActionRight)
               //~ || GetResources()->GetEventTranslator()->Compare(*i, g_kActionLeft)
               //~ )
            //~ {
               //~ l_Continue = true;
            //~ }

            break;
         }
         default:
         {
            break;
         }
      }

      if (l_Continue)
      {
         sf::String l_Name;
         l_Name = m_Name.getString();

         // Remove the trailing underscore
         if (l_Name.getSize() > 0 && l_Name[l_Name.getSize()-1] == sf::String("_"))
         {
            l_Name.erase(l_Name.getSize() - 1, 1);
         }

         // Remove the preceeding "Name: "
         if (l_Name.getSize() > 6)
         {
            l_Name.erase(0, 6);
         }

         (*(GetResources()->GetGameConfigData()))["Challenge"]["PlayerName"] = l_Name;
         GetResources()->SetActiveLevel("HighScores", true);
         UnregisterObject(true);
      }
   }

   // Eat all events
   a_pEventList->clear();
}

void cHighScoreRecorder::Step (uint32_t a_ElapsedMiliSec)
{

}

void cHighScoreRecorder::Collision(cObject* a_pOther)
{
}

void cHighScoreRecorder::Draw(const sf::Vector2<float> & a_rkInterpolationOffset)
{
   GetResources()->GetWindow()->draw(m_Description);
   GetResources()->GetWindow()->draw(m_Name);
}
