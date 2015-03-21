// This is a level to be loaded from main probably

#ifndef ___cChallengeLevel_h___
#define ___cChallengeLevel_h___

#include "cLevel.h"

class cChallengeLevel: public cLevel
{
public:
   cChallengeLevel(
      cResources* a_pResources
      );

   ~cChallengeLevel();

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
