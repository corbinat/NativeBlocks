#ifndef ___cAnimation_h___
#define ___cAnimation_h___

#include "SFML/Graphics.hpp"

#include <list>
#include <stdint.h> // uint32_t

struct sFrame
{
   sf::Sprite m_Sprite;
   sf::Rect<int32_t> m_BoundingBox;
};

class cAnimation
{
public:
   cAnimation();
   ~cAnimation();

   cAnimation(const cAnimation & a_rkOther);
   cAnimation& operator=(const cAnimation & a_rkOther);

   bool AddFrame(
      sf::Texture* a_pTexture,
      uint32_t a_Height,
      uint32_t a_Width,
      uint32_t a_PixelOffsetX,
      uint32_t a_PixelOffsetY,
      sf::Rect<int32_t> a_BoundingBox
      );

   void SetFrame(uint32_t a_FrameNumber);

   void SetSpeed(uint32_t a_Speed);

   void SetPosition(double a_X, double a_Y);

   sf::Sprite* GetCurrentSprite();

   sf::Rect<int32_t> GetBoundingBox();

   void Step (uint32_t a_ElapsedMiliSec);

   bool AnimationHasEnded();

private:

   // This is the speed the animation should play at in frames/second
   uint32_t m_Speed;

   std::list<sFrame> m_Animation;
   std::list<sFrame>::iterator m_pCurrentFrame;

   // How long the current frame has been running
   uint32_t m_FrameTime;

   bool m_AnimationEnded;

};

#endif
