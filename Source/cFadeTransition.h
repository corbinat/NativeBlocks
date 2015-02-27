#ifndef ___cFadeTransition_h___
#define ___cFadeTransition_h___

#include "cObject.h"


class cFadeTransition: public cObject
{
public:

   cFadeTransition(cResources* a_pResources);
   virtual ~cFadeTransition();

   enum eFadeDirection
   {
      kFadeDirectionIn,
      kFadeDirectionOut
   };

   void SetFadeDirection(eFadeDirection a_FadeDirection);
   void SetTransitionTime(uint32_t a_TransitionTimeMs);

   // These functions are overloaded from cObject
   virtual void Draw();
   virtual void Step (uint32_t a_ElapsedMiliSec);

private:

   eFadeDirection m_FadeDirection;
   uint32_t m_TransitionTimeInMs;
   sf::RectangleShape m_Rectangle;
   uint32_t m_ElapsedMiliSec;
   bool m_FirstStep;
};

#endif
