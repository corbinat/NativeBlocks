#include "cPlayer.h"
#include "cBean.h"
#include "cFloor.h"

#include "cResources.h"

#include <algorithm>
#include <iostream>

cPlayer::cPlayer(cResources* a_pResources)
   : cObject(a_pResources),
     m_Initialized(false),
     m_CurrentState(kStateIdle),
     m_RotationState(kRotationUp),
     m_pPivotBean(NULL),
     m_pSwingBean(NULL),
     m_FallingBeans(),
     m_Beans(),
     m_MiliSecPerFall(500),
     m_MiliSecSinceLastFall(0),
     m_FastFall(false),
     m_BeanControlEnabled(false)
{
   // Receive messages when beans finish falling. That way we can know when to
   // stop waiting for them to settle.
   sMessage l_Request;
   l_Request.m_From = GetResources()->GetMessageDispatcher()->AnyID();
   l_Request.m_Category = "Player1";
   l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Value = "BeanSettled";

   std::function<void(sMessage)> l_MessageCallback =
      std::bind(&cPlayer::MessageReceived, this, std::placeholders::_1);

   GetResources()->GetMessageDispatcher()->RegisterForMessages(
      GetUniqueId(),
      l_MessageCallback,
      l_Request
      );

   _StartGame();
}

cPlayer::~cPlayer()
{
   GetResources()->GetMessageDispatcher()->CancelMessages(GetUniqueId());
}

void cPlayer::Event(std::list<sf::Event> * a_pEventList)
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
               _MoveControlledBeans(l_RelativePosition);
            }
            if ((*i).key.code == sf::Keyboard::Left)
            {
               sf::Vector3<double> l_RelativePosition;
               l_RelativePosition.x -=
                  static_cast<double>(GetResources()->GetGridCellSize().x);
               _MoveControlledBeans(l_RelativePosition);
            }
            if ((*i).key.code == sf::Keyboard::Down)
            {
               m_FastFall = true;
            }
            if ((*i).key.code == sf::Keyboard::Space)
            {
               if (m_BeanControlEnabled)
               {
                  if (m_RotationState == kRotationUp)
                  {
                     sf::Vector3<double> l_Position = m_pPivotBean->GetPosition();
                     l_Position.x += GetResources()->GetGridCellSize().x;
                     m_pSwingBean->SetPosition(l_Position, kNormal, false);
                  }
               }
            }

            break;
         }
         case sf::Event::KeyReleased:
         {
            if ((*i).key.code == sf::Keyboard::Down)
            {
               m_FastFall = false;
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

void cPlayer::Step (uint32_t a_ElapsedMiliSec)
{
   if (!m_Initialized)
   {
      _Initialize();
   }

   // State Machine:
   // 1. Move staged beans into play
   // 2. Move beans down, wait, repeat
   // 3. If can't move down, lock in (remove user input)

   // Starting bean responsibily
   // 4. Make beans fall
   //    Beans should send message when they are done falling. Once we receive
   //    a message we wait until the step where we don't receive any more. That
   //    is how we know we're done.

   // 5. Update playing field matrix
   // 6. Look for match
   // 7. If match, go to 4
   // 8. Go to 1

   switch (m_CurrentState)
   {
      case kStateCreateBeans:
      {
         m_pPivotBean = new cBean(GetResources());
         RegisterObject(m_pPivotBean);
         sf::Vector3<double> l_Position = GetPosition();
         l_Position.x += GetResources()->GetGridCellSize().x * 2;
         l_Position.y += GetResources()->GetGridCellSize().y;
         m_pPivotBean->SetPosition(l_Position, kNormal, false);

         m_pSwingBean = new cBean(GetResources());
         RegisterObject(m_pSwingBean);
         l_Position = GetPosition();
         l_Position.x += GetResources()->GetGridCellSize().x * 2;
         m_pSwingBean->SetPosition(l_Position, kNormal, false);

         m_CurrentState = kStateControlBeans;

         break;
      }

      case kStateControlBeans:
      {
         m_BeanControlEnabled = true;

         // Move the beans down
         m_MiliSecSinceLastFall += a_ElapsedMiliSec;
         if (  m_MiliSecSinceLastFall > m_MiliSecPerFall
            || (m_FastFall == true &&  m_MiliSecSinceLastFall > 10)
            )
         {
            m_MiliSecSinceLastFall = 0;

            sf::Vector3<double> l_RelativePosition;
            l_RelativePosition.y +=
               static_cast<double>(GetResources()->GetGridCellSize().y) / 2;
            bool l_MoveSuccess = _MoveControlledBeans(l_RelativePosition);

            // Can't move beans down any more, so lock in.
            if (!l_MoveSuccess)
            {
               m_FastFall = false;
               m_CurrentState = kStateWaitForBeansToSettle;
               m_FallingBeans.push_back(m_pPivotBean);
               m_FallingBeans.push_back(m_pSwingBean);

               m_Beans.push_back(m_pPivotBean);
               m_Beans.push_back(m_pSwingBean);

               m_pPivotBean->Fall();
               m_pSwingBean->Fall();
            }
         }
         break;
      }

      case kStateWaitForBeansToSettle:
      {
         m_BeanControlEnabled = false;
         if (m_FallingBeans.size() == 0)
         {
            m_CurrentState = kStateCreateBeans;
         }
      }

      case kStateCheckForMatches:
      {
      }

      default:
      {
      }
   }
}

void cPlayer::Collision(cObject* a_pOther)
{
}

void cPlayer::MessageReceived(sMessage a_Message)
{
   if (a_Message.m_Value == "BeanSettled" && a_Message.m_Category == "Player1")
   {
      cObject* l_pObject = GetObjectWithId(a_Message.m_From);
      if (l_pObject != NULL)
      {
         for (
            std::vector<cBean*>::iterator i = m_FallingBeans.begin();
            i != m_FallingBeans.end();
            ++i
            )
         {
            if ((*i) == l_pObject)
            {
               m_FallingBeans.erase(i);
               return;
            }
         }
      }
   }
}

void cPlayer::_Initialize()
{
   m_Initialized = true;

   cFloor* l_NewFloor = new cFloor(GetResources());
   RegisterObject(l_NewFloor);

   sf::Vector3<double> l_Position = GetPosition();
   l_Position.y += GetResources()->GetGridCellSize().x * 13;
   l_NewFloor->SetPosition(l_Position, kNormal, false);
}

void cPlayer::_StartGame()
{
   m_CurrentState = kStateCreateBeans;
}

bool cPlayer::_MoveControlledBeans(sf::Vector3<double> a_NewRelativePosition)
{
   if (!m_BeanControlEnabled)
   {
      return false;
   }

   // Make sure we're not going too far left or right
   if (  (a_NewRelativePosition.x + m_pPivotBean->GetPosition().x) > (GetResources()->GetGridCellSize().x * 5)
      || (a_NewRelativePosition.x + m_pSwingBean->GetPosition().x) > (GetResources()->GetGridCellSize().x * 5)
      || (a_NewRelativePosition.x + m_pPivotBean->GetPosition().x) < 0
      || (a_NewRelativePosition.x + m_pSwingBean->GetPosition().x) < 0
      )
   {
      return false;
   }

   // Check for collisions
   sf::Vector3<double> l_Position = m_pPivotBean->GetPosition();
   l_Position += a_NewRelativePosition;
   std::list<cObject*> l_Collisions =
      GetCollisions(m_pPivotBean, l_Position);

   l_Position = m_pSwingBean->GetPosition();
   l_Position += a_NewRelativePosition;
   std::list<cObject*> l_Collisions2 =
      GetCollisions(m_pSwingBean, l_Position);

   // Join the collision lists together
   l_Collisions.splice(l_Collisions.end(), l_Collisions2);

   // Make sure we can move first.
   for (cObject* i : l_Collisions)
   {
      if (i != m_pSwingBean && i != m_pPivotBean)
      {
         return false;
      }
   }

   m_pPivotBean->SetPosition(a_NewRelativePosition, kRelative, false);
   m_pSwingBean->SetPosition(a_NewRelativePosition, kRelative, false);

   return true;
}


