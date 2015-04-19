#include "cHumanPlayer.h"
#include "cResources.h"

#include "Common/Constants.h"

cHumanPlayer::cHumanPlayer(cResources* a_pResources, std::minstd_rand a_RandomNumberEngine, std::string a_Identifier)
   : cPlayer(a_pResources, a_RandomNumberEngine, a_Identifier),
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
            sf::Vector2<uint32_t>* l_pGridCellSize =
               GetResources()->GetActiveLevelData()->GetGridCellSize();

            if (GetResources()->GetEventTranslator()->Compare(*i, g_kActionRight))
            {
               sf::Vector3<double> l_RelativePosition;
               l_RelativePosition.x +=
                  static_cast<double>(l_pGridCellSize->x);
               MoveControlledBeans(l_RelativePosition);
               m_RightKeyDown = true;
            }
            else if (GetResources()->GetEventTranslator()->Compare(*i, g_kActionLeft))
            {
               sf::Vector3<double> l_RelativePosition;
               l_RelativePosition.x -=
                  static_cast<double>(l_pGridCellSize->x);
               MoveControlledBeans(l_RelativePosition);
               m_LeftKeyDown = true;
            }
            else if (GetResources()->GetEventTranslator()->Compare(*i, g_kActionDown))
            {
               SetFastFall(true);
            }
            else if (GetResources()->GetEventTranslator()->Compare(*i, g_kActionRotateCounterClockwise))
            {
               RotateBeans(kRotateCounterClockwise);
            }
            else if (GetResources()->GetEventTranslator()->Compare(*i, g_kActionRotateClockwise))
            {
               RotateBeans(kRotateClockwise);
            }

            break;
         }
         case sf::Event::KeyReleased:
         {
            if (GetResources()->GetEventTranslator()->Compare(*i, g_kActionDown))
            {
               SetFastFall(false);
            }

            if (GetResources()->GetEventTranslator()->Compare(*i, g_kActionLeft))
            {
               m_LeftKeyDown = false;
            }
            if (GetResources()->GetEventTranslator()->Compare(*i, g_kActionRight))
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

         sf::Vector2<uint32_t>* l_pGridCellSize =
            GetResources()->GetActiveLevelData()->GetGridCellSize();

         if (m_LeftKeyDown && !m_RightKeyDown)
         {
            sf::Vector3<double> l_RelativePosition;
            l_RelativePosition.x -=
               static_cast<double>(l_pGridCellSize->x);
            MoveControlledBeans(l_RelativePosition);
         }
         else if (!m_LeftKeyDown && m_RightKeyDown)
         {
            sf::Vector3<double> l_RelativePosition;
            l_RelativePosition.x +=
               static_cast<double>(l_pGridCellSize->x);
            MoveControlledBeans(l_RelativePosition);
         }
      }
   }
}
