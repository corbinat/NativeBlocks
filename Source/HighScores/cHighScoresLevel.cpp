#include "cHighScoresLevel.h"

#include "cHighScoresList.h"
#include "Common/Props/cGrass.h"
#include "Common/Props/cCloud.h"
#include "Common/cFadeTransition.h"
#include "cResources.h"

#include <iostream>

cHighScoresLevel::cHighScoresLevel(
   cResources* a_pResources
   )
   : cLevel(a_pResources)
{
}

cHighScoresLevel::~cHighScoresLevel()
{

}

void cHighScoresLevel::Initialize()
{
   SetGridCellSize(sf::Vector2<uint32_t>(100,100));
   SetBackgroundColor(sf::Color(153,204,204,255));

   //~ GetResources()->GetBackGroundMusic()->openFromFile("Media/Music/GetItBreakdown2.ogg");
   //~ GetResources()->GetBackGroundMusic()->setVolume(30);
   //~ GetResources()->GetBackGroundMusic()->setLoop(true);
   //~ GetResources()->GetBackGroundMusic()->play();

   cHighScoresList * l_List = new cHighScoresList(GetResources());
   l_List->Initialize();

   cFadeTransition * l_pFadeTransition = new cFadeTransition(GetResources());
   l_pFadeTransition->SetFadeDirection(cFadeTransition::kFadeDirectionIn);
   l_pFadeTransition->SetTransitionTime(1000);
   // Make sure the transition is in front of everything
   l_pFadeTransition->SetDepth(-10);
}

void cHighScoresLevel::Step(uint32_t a_ElapsedMiliSec)
{

}

std::string cHighScoresLevel::GetLevelName()
{
   return "HighScores";
}

std::string cHighScoresLevel::GetObjectMapPath()
{
   return "Media/HighScores.bmp";
}

cObject* cHighScoresLevel::PixelToObject(
   sf::Vector2<uint32_t> a_position,
   sf::Color a_Color,
   cResources* a_pResources
   )
{
   cObject * l_pNewObject = NULL;

   if (a_Color.r == 0 && a_Color.g == 255 && a_Color.b == 0)
   {
      l_pNewObject = new cGrass(a_pResources);
      l_pNewObject->SetDepth(100);
   }
   else if (a_Color.r == 0 && a_Color.g == 100 && a_Color.b == 255)
   {
      l_pNewObject = new cCloud(a_pResources);
   }
   if (l_pNewObject != NULL)
   {
      sf::Vector3<double> l_NewPosition(a_position.x * 32, a_position.y * 32, 0);

      // Set position but don't bother checking for collisions
      l_pNewObject->SetPosition(l_NewPosition, kNormal, false);
   }

   return l_pNewObject;
}

void cHighScoresLevel::PixelToTile()
{
}

