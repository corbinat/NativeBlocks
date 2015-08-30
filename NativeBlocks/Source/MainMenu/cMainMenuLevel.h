// This is a level to be loaded from main probably

#ifndef ___cMainMenuLevel_h___
#define ___cMainMenuLevel_h___

#include "cLevel.h"

class cMainMenuLevel: public cLevel
{
public:
   cMainMenuLevel(
      cResources* a_pResources
      );

   ~cMainMenuLevel(){}

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

};

#endif
