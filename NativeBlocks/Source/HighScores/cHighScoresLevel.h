// This is a level to be loaded from main probably

#ifndef ___cHighScoresLevel_h___
#define ___cHighScoresLevel_h___

#include "cLevel.h"

class cHighScoresLevel: public cLevel
{
public:
   cHighScoresLevel(
      cResources* a_pResources
      );

   ~cHighScoresLevel();

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
