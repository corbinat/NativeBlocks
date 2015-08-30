#include "cCreditsLevel.h"

#include "cCreditsString.h"
#include "Common/Props/cGrass.h"
#include "Common/Props/cCloud.h"
#include "Common/cFadeTransition.h"
#include "MainMenu/cTitle.h"
#include "cResources.h"

#include <iostream>

cCreditsLevel::cCreditsLevel(cResources* a_pResources)
   : cLevel(a_pResources)
{
}

cCreditsLevel::~cCreditsLevel()
{

}

void cCreditsLevel::Initialize()
{
   SetGridCellSize(sf::Vector2<uint32_t>(100,100));
   SetBackgroundColor(sf::Color(153,204,204,255));

   sf::Vector3<double> l_Position;

   cTitle * m_pTitle = new cTitle(GetResources());
   l_Position.x = GetResources()->GetWindow()->getSize().x/2 - (m_pTitle->GetBoundingBox().width/2);
   l_Position.y = 30;
   m_pTitle->SetPosition(l_Position, kNormal, false);
   m_pTitle->Initialize();

   cCreditsString * l_List = new cCreditsString(GetResources());
   l_Position.x = GetResources()->GetWindow()->getSize().x/2;
   l_Position.y = 120;
   l_List->SetPosition(l_Position, kNormal, false);
   l_List->Initialize();

   cFadeTransition * l_pFadeTransition = new cFadeTransition(GetResources());
   l_pFadeTransition->SetFadeDirection(cFadeTransition::kFadeDirectionIn);
   l_pFadeTransition->SetTransitionTime(1000);
   // Make sure the transition is in front of everything
   l_pFadeTransition->SetDepth(-10);
}

void cCreditsLevel::Step(uint32_t a_ElapsedMiliSec)
{

}

std::string cCreditsLevel::GetLevelName()
{
   return "Credits";
}

