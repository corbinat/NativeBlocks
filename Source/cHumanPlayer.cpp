#include "cHumanPlayer.h"
#include "cResources.h"

cHumanPlayer::cHumanPlayer(cResources* a_pResources)
   : cPlayer(a_pResources),
     m_KeyRepeatTime(0),
     m_KeyRepeatLimit(150),
     m_LeftKeyDown(false),
     m_RightKeyDown(false)
{
}

cHumanPlayer::~cHumanPlayer()
{
}

void cHumanPlayer::Event(std::list<sf::Event> * a_pEventList)
{
std::list<sf::Event>::iterator i = a_pEventList->begin();
   for (i; i != a_pEventList->end(); ++i)
   {
      switch((*i).type)
      {
         case sf::Event::KeyPressed:
         {
            if ((*i).key.code == sf::Keyboard::Right)
            {
               sf::Vector3<double> l_RelativePosition;
               l_RelativePosition.x +=
                  static_cast<double>(GetResources()->GetGridCellSize().x);
               MoveControlledBeans(l_RelativePosition);
               m_RightKeyDown = true;
            }
            if ((*i).key.code == sf::Keyboard::Left)
            {
               sf::Vector3<double> l_RelativePosition;
               l_RelativePosition.x -=
                  static_cast<double>(GetResources()->GetGridCellSize().x);
               MoveControlledBeans(l_RelativePosition);
               m_LeftKeyDown = true;
            }
            if ((*i).key.code == sf::Keyboard::Down)
            {
               SetFastFall(true);
            }
            if ((*i).key.code == sf::Keyboard::Space)
            {
               RotateBeans(kRotateClockwise);
            }

            break;
         }
         case sf::Event::KeyReleased:
         {
            if ((*i).key.code == sf::Keyboard::Down)
            {
               SetFastFall(false);
            }

            if ((*i).key.code == sf::Keyboard::Left)
            {
               m_LeftKeyDown = false;
            }
            if ((*i).key.code == sf::Keyboard::Right)
            {
               m_RightKeyDown = false;
            }

            if (!m_LeftKeyDown && !m_RightKeyDown)
            {
               m_KeyRepeatTime = 0;
            }

            break;
         }
         default:
         {
            break;
         }
      }
   }
}

void cHumanPlayer::StateChange(ePlayerState a_Old, ePlayerState a_New)
{
   if (a_New == kStateWaitForBeansToSettle)
   {
      m_KeyRepeatTime = 0;
   }
}

void cHumanPlayer::ControlBeans(uint32_t a_ElapsedMiliSec)
{
   if (GetPlayerState() == kStateControlBeans && (m_LeftKeyDown || m_RightKeyDown))
   {
      m_KeyRepeatTime += a_ElapsedMiliSec;
      if (m_KeyRepeatTime > m_KeyRepeatLimit)
      {
         m_KeyRepeatTime = 80;
         if (m_LeftKeyDown && !m_RightKeyDown)
         {
            sf::Vector3<double> l_RelativePosition;
            l_RelativePosition.x -=
               static_cast<double>(GetResources()->GetGridCellSize().x);
            MoveControlledBeans(l_RelativePosition);
         }
         else if (!m_LeftKeyDown && m_RightKeyDown)
         {
            sf::Vector3<double> l_RelativePosition;
            l_RelativePosition.x +=
               static_cast<double>(GetResources()->GetGridCellSize().x);
            MoveControlledBeans(l_RelativePosition);
         }
      }
   }
}
