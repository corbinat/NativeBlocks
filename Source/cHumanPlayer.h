#ifndef ___cHumanPlayer_h___
#define ___cHumanPlayer_h___

#include "cPlayer.h"

class cHumanPlayer : public cPlayer
{
public:
   cHumanPlayer(cResources* a_pResources);
   ~cHumanPlayer();

   // These functions are overloaded from cPlayer
   virtual void Event(std::list<sf::Event> * a_pEventList);

   void StateChange(ePlayerState a_Old, ePlayerState a_New);
   void ControlBeans(uint32_t a_ElapsedMiliSec);

private:

   uint32_t m_KeyRepeatTime;
   const uint32_t m_KeyRepeatLimit;
   bool m_LeftKeyDown;
   bool m_RightKeyDown;

};

#endif
