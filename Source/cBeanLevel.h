// This is a level to be loaded from main probably

#ifndef ___cBeanLevel_h___
#define ___cBeanLevel_h___

#include "cLevel.h"


class cBeanLevel: public cLevel
{
public:
   cBeanLevel(
      cResources* a_pResources
      );

   ~cBeanLevel(){}

   void Initialize();
   void Step (uint32_t a_ElapsedMiliSec);

   std::string GetLevelName();
   std::string GetObjectMapPath();

   cObject* PixelToObject(
      sf::Vector2<uint32_t> a_position,
      sf::Color a_Color,
      cResources* a_pResources
      );

   void PixelToTile();

private:

   std::minstd_rand m_RandomNumberEngine;

};

#endif
