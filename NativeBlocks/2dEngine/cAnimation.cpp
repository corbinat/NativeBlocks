#include "cAnimation.h"
#include "Common.h"
#include <iostream>

cAnimation::cAnimation()
   : m_Speed(8),
     m_Animation(),
     m_pCurrentFrame(),
     m_FrameTime(0),
     m_AnimationEnded(false)
{
}

cAnimation::~cAnimation()
{
}

cAnimation::cAnimation(const cAnimation & a_rkOther)
{
   m_Speed = a_rkOther.m_Speed;
   m_Animation = a_rkOther.m_Animation;
   m_pCurrentFrame = m_Animation.begin();
   m_FrameTime = a_rkOther.m_FrameTime;
   m_AnimationEnded = a_rkOther.m_AnimationEnded;
}

cAnimation& cAnimation::operator=(const cAnimation & a_rkOther)
{
   m_Speed = a_rkOther.m_Speed;
   m_Animation = a_rkOther.m_Animation;
   m_pCurrentFrame = m_Animation.begin();
   m_FrameTime = a_rkOther.m_FrameTime;
   m_AnimationEnded = a_rkOther.m_AnimationEnded;

   return *this;
}

bool cAnimation::AddFrame(
   sf::Texture* a_pTexture,
   uint32_t a_Height,
   uint32_t a_Width,
   uint32_t a_PixelOffsetX,
   uint32_t a_PixelOffsetY,
   sf::Rect<int32_t> a_BoundingBox
   )
{
   if (a_pTexture == NULL)
   {
      DebugPrintf("Texture is NULL\n");
      return false;
   }

   sf::Rect<int> l_FrameRect(
      a_PixelOffsetX,
      a_PixelOffsetY,
      a_Width,
      a_Height
      );

   sf::Sprite l_Sprite(*a_pTexture, l_FrameRect);

   sFrame l_NewFrame = {l_Sprite, a_BoundingBox};
   m_Animation.push_back(l_NewFrame);

   m_pCurrentFrame = m_Animation.begin();

   return true;
}

void cAnimation::SetFrame(uint32_t a_FrameNumber)
{
   m_pCurrentFrame = m_Animation.begin();
   for (uint32_t i = 0; i < a_FrameNumber; ++i)
   {
      if (m_pCurrentFrame != m_Animation.end())
      {
         ++m_pCurrentFrame;
      }
   }
   m_AnimationEnded = false;
}

void cAnimation::SetSpeed(uint32_t a_Speed)
{
   m_Speed = a_Speed;
}

void cAnimation::SetPosition(double a_X, double a_Y)
{
   for (auto i = m_Animation.begin(); i != m_Animation.end(); ++i)
   {
      i->m_Sprite.setPosition(a_X, a_Y);
   }
}

sf::Sprite* cAnimation::GetCurrentSprite()
{
   return &(m_pCurrentFrame->m_Sprite);
}

sf::Rect<int32_t> cAnimation::GetBoundingBox()
{
   return m_pCurrentFrame->m_BoundingBox;
}

void cAnimation::Step (uint32_t a_ElapsedMiliSec)
{
   // Calculate how many miliseconds until we should change frames
   uint32_t l_Bound = 1000 / m_Speed;
   m_FrameTime += a_ElapsedMiliSec;

   while (m_FrameTime > l_Bound)
   {
      m_FrameTime -= l_Bound;
      ++m_pCurrentFrame;
      if (m_pCurrentFrame == m_Animation.end())
      {
         m_AnimationEnded = true;
         m_pCurrentFrame = m_Animation.end();
         --m_pCurrentFrame;
      }
   }

}

bool cAnimation::AnimationHasEnded()
{
   return m_AnimationEnded;
}
