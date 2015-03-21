#include "cChallengeLevel.h"
#include "cChallengeDisplay.h"
#include "Common/Props/cGrass.h"
#include "Common/Props/cCloud.h"
#include "Common/cFadeTransition.h"
#include "cResources.h"

#include <iostream>

cChallengeLevel::cChallengeLevel(
   cResources* a_pResources
   )
   : cLevel(a_pResources)
{
}

cChallengeLevel::~cChallengeLevel()
{

}

void cChallengeLevel::Initialize()
{
   SetGridCellSize(sf::Vector2<uint32_t>(100,100));
   SetBackgroundColor(sf::Color(153,204,204,255));

   cFadeTransition * l_pFadeTransition = new cFadeTransition(GetResources());
   l_pFadeTransition->SetFadeDirection(cFadeTransition::kFadeDirectionIn);
   l_pFadeTransition->SetTransitionTime(1000);
   // Make sure the transition is in front of everything
   l_pFadeTransition->SetDepth(-10);
}

void cChallengeLevel::Step(uint32_t a_ElapsedMiliSec)
{

}

std::string cChallengeLevel::GetLevelName()
{
   return "Challenge";
}

std::string cChallengeLevel::GetObjectMapPath()
{
   return "Media/Challenge.bmp";
}

cObject* cChallengeLevel::PixelToObject(
   sf::Vector2<uint32_t> a_position,
   sf::Color a_Color,
   cResources* a_pResources
   )
{
   cObject * l_pNewObject = NULL;

   if (a_Color.r == 0 && a_Color.g == 0 && a_Color.b == 0)
   {
      l_pNewObject = new cChallengeDisplay(a_pResources);
   }
   else if (a_Color.r == 0 && a_Color.g == 255 && a_Color.b == 0)
   {
      l_pNewObject = new cGrass(a_pResources);
      l_pNewObject->SetDepth(100);
   }
   //~ else if (a_Color.r == 0 && a_Color.g == 0 && a_Color.b == 255)
   //~ {
      //~ l_pNewObject = new cCloud(a_pResources);
   //~ }
   //~ else if (a_Color.r == 200 && a_Color.g == 0 && a_Color.b == 0)
   //~ {
      //~ l_pNewObject = new cMainMenu(a_pResources);
   //~ }

   if (l_pNewObject != NULL)
   {
      sf::Vector3<double> l_NewPosition(a_position.x * 32, a_position.y * 32, 0);

      // Set position but don't bother checking for collisions
      l_pNewObject->SetPosition(l_NewPosition, kNormal, false);
   }

   return l_pNewObject;
}

void cChallengeLevel::PixelToTile()
{
}

