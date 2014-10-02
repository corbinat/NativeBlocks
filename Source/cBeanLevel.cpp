#include "cBeanLevel.h"
#include "cHumanPlayer.h"
#include "cAiPlayer.h"
#include "cStagingObserver.h"

#include "cResources.h"

#include <iostream>

cBeanLevel::cBeanLevel(
   std::string a_FileName,
   std::string a_LevelName,
   cResources* a_pResources
   )
   : cLevel(a_FileName, a_LevelName, a_pResources),
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
}

void cBeanLevel::Step(uint32_t a_ElapsedMiliSec)
{

}

cObject* cBeanLevel::PixelToObject(
   std::string a_LevelName,
   sf::Vector2<uint32_t> a_position,
   sf::Color a_Color,
   cResources* a_pResources
   )
{
   if (a_Color.r == 0 && a_Color.g == 0 && a_Color.b == 0)
   {
      cHumanPlayer* l_pPlayer = new cHumanPlayer(a_pResources, m_RandomNumberEngine, "Player1");
      return l_pPlayer;
      //return NULL;
   }
   else if (a_Color.r == 100 && a_Color.g == 100 && a_Color.b == 100)
   {
      cStagingObserver* l_pPlayer1Staging = new cStagingObserver(a_pResources, m_RandomNumberEngine, "Player1");
      return l_pPlayer1Staging;
      //return NULL;
   }

   else if (a_Color.r == 255 && a_Color.g == 0 && a_Color.b == 0)
   {
      cAiPlayer* l_pPlayer = new cAiPlayer(a_pResources, m_RandomNumberEngine, "Player2");
      return l_pPlayer;
   }
   else if (a_Color.r == 100 && a_Color.g == 0 && a_Color.b == 0)
   {
      cStagingObserver* l_pPlayer2Staging = new cStagingObserver(a_pResources, m_RandomNumberEngine, "Player2");
      return l_pPlayer2Staging;
      //return NULL;
   }
   else
   {
      return NULL;
   }
}

void cBeanLevel::PixelToTile()
{
}

