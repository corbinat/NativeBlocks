// This is a level to be loaded from main probably

#ifndef ___cCreditsLevel_h___
#define ___cCreditsLevel_h___

#include "cLevel.h"

class cCreditsLevel: public cLevel
{
public:
   cCreditsLevel(
      cResources* a_pResources
      );

   ~cCreditsLevel();

   void Initialize();
   void Step (uint32_t a_ElapsedMiliSec);

   std::string GetLevelName();

   //std::string GetObjectMapPath();

   //~ cObject* PixelToObject(
      //~ sf::Vector2<uint32_t> a_position,
      //~ sf::Color a_Color,
      //~ cResources* a_pResources
      //~ );
//~
   //~ void PixelToTile();

private:

};

#endif
