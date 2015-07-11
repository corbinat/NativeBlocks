#include "cHumanPlayer.h"
#include "cResources.h"

#include "Common/Constants.h"

cHumanPlayer::cHumanPlayer(cResources* a_pResources, std::minstd_rand a_RandomNumberEngine, std::string a_Identifier, uint32_t a_SpeedLevel)
   : cPlayer(a_pResources, a_RandomNumberEngine, a_Identifier, a_SpeedLevel),
     m_KeyRepeatTime(0),
     m_KeyRepeatLimit(150),
     m_LeftKeyDown(false),
     m_RightKeyDown(false),
     m_Paused(false)
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
         case sf::Event::MouseButtonPressed:
         case sf::Event::JoystickButtonPressed:
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
            else if (GetResources()->GetEventTranslator()->Compare(*i, g_kActionPause))
            {
               if (GetPlayerState() != kStateIdle)
               {
                  m_Paused = !m_Paused;
                  sMessage l_Message;
                  l_Message.m_From = GetUniqueId();
                  l_Message.m_Category = GetResources()->GetMessageDispatcher()->Any();
                  l_Message.m_Key = "PauseState";
                  l_Message.m_Value = m_Paused ? "Pause" : "Unpause";
                  GetResources()->GetMessageDispatcher()->PostMessage(l_Message);
               }
            }

            break;
         }
         case sf::Event::KeyReleased:
         case sf::Event::MouseButtonReleased:
         case sf::Event::JoystickButtonReleased:
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
