#include "cBeanLevel.h"
#include "cHumanPlayer.h"
#include "cAiPlayer.h"
#include "cStagingObserver.h"
#include "Common/Props/cGrass.h"
#include "cCountdownStart.h"
#include "cFinalMenu.h"

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
   //~ std::random_device l_RandomSeeder;
   //~ uint32_t l_Seed = l_RandomSeeder();
   //~ std::minstd_rand l_Random(l_Seed);
   std::minstd_rand l_Random(std::chrono::high_resolution_clock::now().time_since_epoch().count());

   m_RandomNumberEngine = l_Random;

   // Change default level settings
   SetGridCellSize(sf::Vector2<uint32_t>(32,32));
   SetBackgroundColor(sf::Color(153,204,204,255));
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
      std::string l_Player1Option = GetResources()->GetGameConfigData()->GetProperty("Player1");

      if (l_Player1Option == "Human")
      {
         l_pNewObject = new cHumanPlayer(a_pResources, m_RandomNumberEngine, "Player1");
      }
      else
      {
         l_pNewObject = new cAiPlayer(a_pResources, m_RandomNumberEngine, "Player1", l_Player1Option);
      }

   }
   else if (a_Color.r == 100 && a_Color.g == 100 && a_Color.b == 100)
   {
      l_pNewObject = new cStagingObserver(a_pResources, m_RandomNumberEngine, "Player1");
   }

   else if (a_Color.r == 255 && a_Color.g == 0 && a_Color.b == 0)
   {
      std::string l_Player2Option = GetResources()->GetGameConfigData()->GetProperty("Player2");
      l_pNewObject = new cAiPlayer(a_pResources, m_RandomNumberEngine, "Player2", l_Player2Option);
   }
   else if (a_Color.r == 100 && a_Color.g == 0 && a_Color.b == 0)
   {
      l_pNewObject = new cStagingObserver(a_pResources, m_RandomNumberEngine, "Player2");
   }
   else if (a_Color.r == 0 && a_Color.g == 255 && a_Color.b == 0)
   {
      l_pNewObject = new cGrass(a_pResources);
   }
   else if (a_Color.r == 0 && a_Color.g ==0 && a_Color.b == 255)
   {
      l_pNewObject = new cCountdownStart(a_pResources);
   }
   else if (a_Color.r == 255 && a_Color.g ==0 && a_Color.b == 255)
   {
      l_pNewObject = new cFinalMenu(a_pResources);
   }

   if (l_pNewObject != NULL)
   {
      sf::Vector3<double> l_NewPosition(a_position.x * 32, a_position.y * 32, 0);

      // Set position but don't bother checking for collisions
      l_pNewObject->SetPosition(l_NewPosition, kNormal, false);
   }

   return l_pNewObject;
}

void cBeanLevel::PixelToTile()
{
}
