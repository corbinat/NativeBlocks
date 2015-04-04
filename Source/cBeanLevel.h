// This is a level to be loaded from main probably

#ifndef ___cBeanLevel_h___
#define ___cBeanLevel_h___

#include "cLevel.h"

class cPlayer;

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

   // This is kind of hacky. I save the first player created to this pointer
   // so that when I get to the second player I can let them know of each other.
   cPlayer* m_pOpponent;

};

#endif
