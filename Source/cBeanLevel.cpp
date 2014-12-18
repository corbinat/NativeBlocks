#include "cBeanLevel.h"
#include "cHumanPlayer.h"
#include "cAiPlayer.h"
#include "cStagingObserver.h"

#include "cResources.h"

#include <iostream>

cBeanLevel::cBeanLevel(
   cResources* a_pResources
   )
   : cLevel(a_pResources),
     m_RandomNumberEngine()
{
}

void cBeanLevel::Initialize()
{
   // Generate a new bean order random number generator. This will be passed to
   // all objects such that they generate beans in the same order.
   std::random_device l_RandomSeeder;
   uint32_t l_Seed = l_RandomSeeder();
   std::minstd_rand l_Random(l_Seed);
   m_RandomNumberEngine = l_Random;

   // Change default level settings
   SetGridCellSize(sf::Vector2<uint32_t>(32,32));
   SetBackgroundColor(sf::Color(150,150,150,255));
}

void cBeanLevel::Step(uint32_t a_ElapsedMiliSec)
{

}

std::string cBeanLevel::GetLevelName()
{
   return "Level1";
}

std::string cBeanLevel::GetObjectMapPath()
{
   return "Media/Level1.bmp";
}

cObject* cBeanLevel::PixelToObject(
   sf::Vector2<uint32_t> a_position,
   sf::Color a_Color,
   cResources* a_pResources
   )
{
   cObject * l_pNewObject = NULL;

   if (a_Color.r == 0 && a_Color.g == 0 && a_Color.b == 0)
   {
      //cHumanPlayer* l_pPlayer = new cHumanPlayer(a_pResources, m_RandomNumberEngine, "Player1");
      l_pNewObject = new cAiPlayer(a_pResources, m_RandomNumberEngine, "Player1", kAiPersonalityHard);
   }
   else if (a_Color.r == 100 && a_Color.g == 100 && a_Color.b == 100)
   {
      l_pNewObject = new cStagingObserver(a_pResources, m_RandomNumberEngine, "Player1");
   }

   else if (a_Color.r == 255 && a_Color.g == 0 && a_Color.b == 0)
   {
      //cAiPlayer* l_pPlayer = new cAiPlayer(a_pResources, m_RandomNumberEngine, "Player2", kAiPersonalityHard);
      l_pNewObject = new cAiPlayer(a_pResources, m_RandomNumberEngine, "Player2", kAiPersonalityMedium);
   }
   else if (a_Color.r == 100 && a_Color.g == 0 && a_Color.b == 0)
   {
      l_pNewObject = new cStagingObserver(a_pResources, m_RandomNumberEngine, "Player2");
   }

   if (l_pNewObject != NULL)
   {
      sf::Vector3<double> l_NewPosition(a_position.x * 32, a_position.y * 32, 0);

      // Set position but don't bother checking for collisions
      l_pNewObject->SetPosition(l_NewPosition, kNormal, false);
   }
}

void cBeanLevel::PixelToTile()
{
}

