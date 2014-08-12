// This is a level to be loaded from main probably

#ifndef ___cBeanLevel_h___
#define ___cBeanLevel_h___

#include "cLevel.h"


class cBeanLevel: public cLevel
{
public:
   cBeanLevel(
      std::string a_FileName,
      std::string a_LevelName,
      cResources* a_pResources
      );

   ~cBeanLevel(){}

   void Step (uint32_t a_ElapsedMiliSec);

   cObject* PixelToObject(
      std::string a_LevelName,
      sf::Vector2<uint32_t> a_position,
      sf::Color a_Color,
      cResources* a_pResources
      );

   void PixelToTile();

private:

};

#endif
