#include "cBeanLevel.h"
#include "cPlayer.h"

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
      cPlayer* l_pPlayer = new cPlayer(a_pResources);
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

