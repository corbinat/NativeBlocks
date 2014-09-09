#include "cBeanLevel.h"
#include "cHumanPlayer.h"
#include "cAiPlayer.h"

#include "cResources.h"

#include <iostream>

cBeanLevel::cBeanLevel(
   std::string a_FileName,
   std::string a_LevelName,
   cResources* a_pResources
   )
   : cLevel(a_FileName, a_LevelName, a_pResources)
{
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
      cHumanPlayer* l_pPlayer = new cHumanPlayer(a_pResources);
      return l_pPlayer;
      //return NULL;
   }
   else if (a_Color.r == 255 && a_Color.g == 0 && a_Color.b == 0)
   {
      cAiPlayer* l_pPlayer = new cAiPlayer(a_pResources);
      return l_pPlayer;
   }
   else
   {
      return NULL;
   }
}

void cBeanLevel::PixelToTile()
{
}

