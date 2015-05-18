#include "cFadeTransition.h"
#include <iostream>

cFadeTransition::cFadeTransition(cResources* a_pResources)
   : cObject(a_pResources),
     m_FadeDirection(kFadeDirectionIn),
     m_TransitionTimeInMs(0),
     m_Rectangle(),
     m_ElapsedMiliSec(0),
     m_FirstStep(true)
{
   SetType("FadeTransition");
   SetSolid(true);
   SetCollidable(false);

   sf::Vector2f l_Size;
   l_Size.x = GetResources()->GetWindow()->getSize().x;
   l_Size.y = GetResources()->GetWindow()->getSize().y;
   m_Rectangle.setSize(l_Size);

   // Figure out where to position the transition. Take into account views
   sf::View l_View = GetResources()->GetWindow()->getView();
   float l_X = l_View.getCenter().x - l_View.getSize().x / 2;
   float l_Y = l_View.getCenter().y - l_View.getSize().y / 2;
   m_Rectangle.setPosition(l_X, l_Y);
}

cFadeTransition::~cFadeTransition()
{
}

void cFadeTransition::SetFadeDirection(eFadeDirection a_FadeDirection)
{
   m_FadeDirection = a_FadeDirection;
}

void cFadeTransition::SetTransitionTime(uint32_t a_TransitionTimeMs)
{
   m_TransitionTimeInMs = a_TransitionTimeMs;
}

void cFadeTransition::Draw()
{
   // Do all of this in draw so that we don't have one quick frame where the
   // color isn't set right
   uint8_t l_Alpha;
   if (m_FadeDirection == kFadeDirectionIn)
   {
      l_Alpha =
         ((m_TransitionTimeInMs - m_ElapsedMiliSec) / static_cast<double>(m_TransitionTimeInMs)) * 255;
   }
   else
   {
      l_Alpha = 255
         - ((m_TransitionTimeInMs - m_ElapsedMiliSec) / static_cast<double>(m_TransitionTimeInMs))
         * 255;
   }

   sf::Color l_Color(0,0,0,l_Alpha);
   m_Rectangle.setFillColor(l_Color);
   GetResources()->GetWindow()->draw(m_Rectangle);
}


void cFadeTransition::Step (uint32_t a_ElapsedMiliSec)
{
   // Don't count the first step
   if (!m_FirstStep)
   {
      m_ElapsedMiliSec += a_ElapsedMiliSec;
   }
   else
   {
      m_FirstStep = false;
   }

   if (m_ElapsedMiliSec > m_TransitionTimeInMs)
   {
      UnregisterObject(true);
   }
}

