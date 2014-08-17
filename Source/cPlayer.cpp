#include "cPlayer.h"
#include "cBean.h"
#include "cFloor.h"
#include "cWall.h"

#include "cResources.h"

#include <algorithm>
#include <iostream>
#include <unordered_set>

cPlayer::cPlayer(cResources* a_pResources)
   : cObject(a_pResources),
     m_Initialized(false),
     m_CurrentState(kStateIdle),
     m_RotationState(kRotationStateUp),
     m_pPivotBean(NULL),
     m_pSwingBean(NULL),
     m_FallingBeans(),
     m_NewBeans(),
     m_Beans(6, std::vector<cBean*>(13, NULL)),
     m_MiliSecPerFall(500),
     m_MiliSecSinceLastFall(0),
     m_RestingBeanTimer(0),
     m_RestingLimit(500),
     m_BeanIsResting(false),
     m_FastFall(false),
     m_BeanControlEnabled(false),
     m_KeyRepeatTime(0),
     m_KeyRepeatLimit(80),
     m_LeftKeyDown(false),
     m_RightKeyDown(false),
     m_TotalSettleTime(0),
     m_MinSettleTime(200)
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
               m_RightKeyDown = true;
            }
            if ((*i).key.code == sf::Keyboard::Left)
            {
               sf::Vector3<double> l_RelativePosition;
               l_RelativePosition.x -=
                  static_cast<double>(GetResources()->GetGridCellSize().x);
               _MoveControlledBeans(l_RelativePosition);
               m_LeftKeyDown = true;
            }
            if ((*i).key.code == sf::Keyboard::Down)
            {
               m_FastFall = true;
            }
            if ((*i).key.code == sf::Keyboard::Space)
            {
               _RotateBeans(kRotateClockwise);
            }

            break;
         }
         case sf::Event::KeyReleased:
         {
            if ((*i).key.code == sf::Keyboard::Down)
            {
               m_FastFall = false;
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

         if (m_BeanIsResting)
         {
            m_RestingBeanTimer += a_ElapsedMiliSec;

            if (m_RestingBeanTimer > m_RestingLimit || m_FastFall)
            {
               // Clean up and lock in beans
               m_BeanIsResting = false;
               m_RestingBeanTimer = 0;
               m_CurrentState = kStateWaitForBeansToSettle;
               m_RotationState = kRotationStateUp;
               m_KeyRepeatTime = 0;
               m_FallingBeans.push_back(m_pPivotBean);
               m_FallingBeans.push_back(m_pSwingBean);

               m_pPivotBean->Fall();
               m_pSwingBean->Fall();
            }
         }
         else
         {
            // Bean is not resting so move it down
            m_MiliSecSinceLastFall += a_ElapsedMiliSec;
            if (  m_MiliSecSinceLastFall > m_MiliSecPerFall
               || (m_FastFall == true &&  m_MiliSecSinceLastFall > 15)
               )
            {
               m_MiliSecSinceLastFall = 0;

               sf::Vector3<double> l_RelativePosition;
               l_RelativePosition.y +=
                  static_cast<double>(GetResources()->GetGridCellSize().y) / 2;
               bool l_MoveSuccess = _MoveControlledBeans(l_RelativePosition);

               if (l_MoveSuccess)
               {
                  // Move was successfull. See if bean is resting now. If so,
                  // start lock-in timer
                  if (_BeansAreResting())
                  {
                     m_BeanIsResting = true;
                  }
               }
               else
               {
                  std::cout << "Shouldn't ever get here..." << std::endl;
                  m_BeanIsResting = true;
               }
            }
         }

         if (m_CurrentState == kStateControlBeans && (m_LeftKeyDown || m_RightKeyDown))
         {
            m_KeyRepeatTime += a_ElapsedMiliSec;
            if (m_KeyRepeatTime > m_KeyRepeatLimit)
            {
               m_KeyRepeatTime = 0;
               if (m_LeftKeyDown && !m_RightKeyDown)
               {
                  sf::Vector3<double> l_RelativePosition;
                  l_RelativePosition.x -=
                     static_cast<double>(GetResources()->GetGridCellSize().x);
                  _MoveControlledBeans(l_RelativePosition);
               }
               else if (!m_LeftKeyDown && m_RightKeyDown)
               {
                  sf::Vector3<double> l_RelativePosition;
                  l_RelativePosition.x +=
                     static_cast<double>(GetResources()->GetGridCellSize().x);
                  _MoveControlledBeans(l_RelativePosition);
               }
            }
         }
         break;
      }

      case kStateWaitForBeansToSettle:
      {
         m_TotalSettleTime += a_ElapsedMiliSec;
         m_BeanControlEnabled = false;
         std::cout << "Wait to settle: " << m_FallingBeans.size() << std::endl;
         if (m_FallingBeans.size() == 0 && m_TotalSettleTime > m_MinSettleTime)
         {
            m_TotalSettleTime = 0;
            m_CurrentState = kStateCheckForMatches;
         }
         break;
      }

      case kStateCheckForMatches:
      {
         // Find neighbors
         for (auto i = m_NewBeans.begin(); i != m_NewBeans.end(); ++i)
         {
            double l_X =
               ((*i)->GetPosition().x - GetPosition().x) / GetResources()->GetGridCellSize().x;

            double l_Y =
               ((*i)->GetPosition().y - GetPosition().y) / GetResources()->GetGridCellSize().y;

            std::cout << "Neighbor (" << l_X << "," << l_Y << ")" << std::endl;

            // Check beans around this one. If same color, add connections
            if (l_X > 0)
            {
               cBean* l_pNeighbor = m_Beans[l_X - 1][l_Y];
               if (l_pNeighbor != NULL && l_pNeighbor->GetColor() == (*i)->GetColor())
               {
                  (*i)->AddConnection(l_pNeighbor);
               }
            }
            if (l_X < 5)
            {
               cBean* l_pNeighbor = m_Beans[l_X + 1][l_Y];
               if (l_pNeighbor != NULL && l_pNeighbor->GetColor() == (*i)->GetColor())
               {
                  (*i)->AddConnection(l_pNeighbor);
               }
            }
            if (l_Y > 0)
            {
               cBean* l_pNeighbor = m_Beans[l_X][l_Y - 1];
               if (l_pNeighbor != NULL && l_pNeighbor->GetColor() == (*i)->GetColor())
               {
                  (*i)->AddConnection(l_pNeighbor);
               }
            }
            if (l_Y < 12)
            {
               cBean* l_pNeighbor = m_Beans[l_X][l_Y + 1];
               if (l_pNeighbor != NULL && l_pNeighbor->GetColor() == (*i)->GetColor())
               {
                  (*i)->AddConnection(l_pNeighbor);
               }
            }
         }

         // Check for connections
         for (auto i = m_NewBeans.begin(); i != m_NewBeans.end(); ++i)
         {
            if ((*i)->IsExploding())
            {
               continue;
            }

            std::unordered_set<cBean*> l_FullConnections = (*i)->CountConnections();

            std::cout << "Connections: " << l_FullConnections.size() << std::endl;
            if (l_FullConnections.size() > 3)
            {
               for (cBean* l_pConnection : l_FullConnections)
               {
                  double l_DeleteX =
                     (l_pConnection->GetPosition().x - GetPosition().x) / GetResources()->GetGridCellSize().x;

                  double l_DeleteY =
                     (l_pConnection->GetPosition().y - GetPosition().y) / GetResources()->GetGridCellSize().y;

                  m_Beans[l_DeleteX][l_DeleteY] = NULL;

                  std::cout << "Exploding " << l_DeleteX << "," << l_DeleteY << std::endl;

                  l_pConnection->Explode();

                  // Don't wait on this bean to fall because it is exploding
                  std::list<cBean*>::iterator l_Find =
                     std::find(
                        m_FallingBeans.begin(),
                        m_FallingBeans.end(),
                        l_pConnection
                        );

                  if (l_Find != m_FallingBeans.end())
                  {
                     m_FallingBeans.erase(l_Find);
                  }

                  // Make all above beans fall.
                  for (l_DeleteY = l_DeleteY - 1;
                       l_DeleteY >= 0 && m_Beans[l_DeleteX][l_DeleteY] != NULL;
                       --l_DeleteY
                      )
                  {
                     std::cout << "Falling (" << l_DeleteX << "," << l_DeleteY << ")" << std::endl;
                     if (!m_Beans[l_DeleteX][l_DeleteY]->IsExploding())
                     {
                        m_FallingBeans.push_back(m_Beans[l_DeleteX][l_DeleteY]);
                     }
                     m_Beans[l_DeleteX][l_DeleteY] = NULL;
                  }

               }
               m_CurrentState = kStateWaitForBeansToSettle;
            }

         }

         for (auto i = m_FallingBeans.begin(); i != m_FallingBeans.end(); ++i)
         {
            (*i)->Fall();
         }

         m_NewBeans.clear();

         // If the state hasn't changed to wait for beans to settle again, then
         // start next bean drop
         if (m_CurrentState == kStateCheckForMatches)
         {
            m_CurrentState = kStateCreateBeans;
         }

      break;
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
            std::list<cBean*>::iterator i = m_FallingBeans.begin();
            i != m_FallingBeans.end();
            ++i
            )
         {
            if ((*i) == l_pObject)
            {
               std::cout << "Bean Settled" << std::endl;
               double l_X =
                  ((*i)->GetPosition().x - GetPosition().x) / GetResources()->GetGridCellSize().x;

               double l_Y =
                  ((*i)->GetPosition().y - GetPosition().y) / GetResources()->GetGridCellSize().y;

               m_Beans[l_X][l_Y] = *i;
               m_NewBeans.push_back(*i);
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
   l_Position.y += GetResources()->GetGridCellSize().y * 13;
   l_NewFloor->SetPosition(l_Position, kNormal, false);

   cWall* l_NewWall = new cWall(GetResources());
   RegisterObject(l_NewWall);
   l_Position = GetPosition();
   l_Position.x -= GetResources()->GetGridCellSize().x;
   l_Position.y += GetResources()->GetGridCellSize().y;
   l_NewWall->SetPosition(l_Position, kNormal, false);

   l_NewWall = new cWall(GetResources());
   RegisterObject(l_NewWall);
   l_Position = GetPosition();
   l_Position.x += GetResources()->GetGridCellSize().x * 6;
   l_Position.y += GetResources()->GetGridCellSize().y;
   l_NewWall->SetPosition(l_Position, kNormal, false);

   l_NewWall = new cWall(GetResources());
   RegisterObject(l_NewWall);
   l_Position = GetPosition();
   l_Position.x -= GetResources()->GetGridCellSize().x;
   l_Position.y += l_NewWall->GetBoundingBox().height + GetResources()->GetGridCellSize().y;
   l_NewWall->SetPosition(l_Position, kNormal, false);

   l_NewWall = new cWall(GetResources());
   RegisterObject(l_NewWall);
   l_Position = GetPosition();
   l_Position.x += GetResources()->GetGridCellSize().x * 6;
   l_Position.y += l_NewWall->GetBoundingBox().height + GetResources()->GetGridCellSize().y;
   l_NewWall->SetPosition(l_Position, kNormal, false);
}

void cPlayer::_StartGame()
{
   m_CurrentState = kStateCreateBeans;
}

bool cPlayer::_BeansAreResting()
{
   if (  m_RotationState == kRotationStateUp
      || m_RotationState == kRotationStateLeft
      || m_RotationState == kRotationStateRight
      )
   {
      sf::Vector3<double> l_Position = m_pPivotBean->GetPosition();
      l_Position.y += GetResources()->GetGridCellSize().y / 2;
      std::list<cObject*> l_Collisions =
         GetCollisions(m_pPivotBean, l_Position);

      if (l_Collisions.size() > 0)
      {
         return true;
      }
   }

   if (  m_RotationState == kRotationStateDown
      || m_RotationState == kRotationStateLeft
      || m_RotationState == kRotationStateRight
      )
   {
      sf::Vector3<double> l_Position = m_pSwingBean->GetPosition();
      l_Position.y += GetResources()->GetGridCellSize().y / 2;
      std::list<cObject*> l_Collisions =
         GetCollisions(m_pSwingBean, l_Position);

      if (l_Collisions.size() > 0)
      {
         return true;
      }
   }

   return false;
}


bool cPlayer::_MoveControlledBeans(sf::Vector3<double> a_NewRelativePosition)
{
   if (!m_BeanControlEnabled)
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

   if (_BeansAreResting())
   {
      m_BeanIsResting = true;
   }
   else
   {
      m_BeanIsResting = false;
   }

   return true;
}

void cPlayer::_RotateBeans(eRotationDirection a_Rotation)
{
   if (!m_BeanControlEnabled)
   {
      return;
   }

   sf::Vector3<double> l_Position = m_pPivotBean->GetPosition();
   sf::Vector3<double> l_OppositePosition = m_pPivotBean->GetPosition();
   eRotationState l_NewState;

   if (  m_RotationState == kRotationStateUp && a_Rotation == kRotateClockwise
      || m_RotationState == kRotationStateDown && a_Rotation == kRotateCounterClockwise
      )
   {
      // Check for collisions to the right of the pivot point
      l_Position.x += GetResources()->GetGridCellSize().x;
      l_OppositePosition.x -= GetResources()->GetGridCellSize().x;
      l_NewState = kRotationStateRight;
   }
   else if (  m_RotationState == kRotationStateRight && a_Rotation == kRotateClockwise
           || m_RotationState == kRotationStateLeft && a_Rotation == kRotateCounterClockwise
           )
   {
      // Check for collisions under the pivot point
      l_Position.y += GetResources()->GetGridCellSize().y;
      l_OppositePosition.y -= GetResources()->GetGridCellSize().y;
      l_NewState = kRotationStateDown;
   }
   else if (  m_RotationState == kRotationStateDown && a_Rotation == kRotateClockwise
           || m_RotationState == kRotationStateUp && a_Rotation == kRotateCounterClockwise
           )
   {
      // Check for collisions left of the pivot point
      l_Position.x -= GetResources()->GetGridCellSize().x;
      l_OppositePosition.x += GetResources()->GetGridCellSize().x;
      l_NewState = kRotationStateLeft;
   }
   else if (  m_RotationState == kRotationStateLeft && a_Rotation == kRotateClockwise
           || m_RotationState == kRotationStateRight && a_Rotation == kRotateCounterClockwise
           )
   {
      // Check for collisions above the pivot point
      l_Position.y -= GetResources()->GetGridCellSize().y;
      l_OppositePosition.y += GetResources()->GetGridCellSize().y;
      l_NewState = kRotationStateUp;
   }

   std::list<cObject*> l_Collisions =
      GetCollisions(m_pPivotBean, l_Position);

   if (l_Collisions.size() == 0)
   {
      m_pSwingBean->SetPosition(l_Position, kNormal, false);
   }
   else
   {
      // There is something in the rotation space, see if we can scootch over.
      // See if we have to scootch a full space or half space when dealing with
      // y.
      sf::Vector3<double> l_NewSwingPosition = m_pPivotBean->GetPosition();
      if (
           (static_cast<uint32_t>(m_pPivotBean->GetPosition().y)
           % GetResources()->GetGridCellSize().y
           )
           != 0
         )
      {
         l_OppositePosition.y += GetResources()->GetGridCellSize().y / 2;
         l_NewSwingPosition.y += GetResources()->GetGridCellSize().y / 2;
      }


      l_Collisions =
         GetCollisions(m_pPivotBean, l_OppositePosition);
      if (l_Collisions.size() == 0)
      {
         m_pSwingBean->SetPosition(l_NewSwingPosition, kNormal, false);
         m_pPivotBean->SetPosition(l_OppositePosition, kNormal, false);
      }
   }

   if (l_Collisions.size() == 0)
   {
      m_RotationState = l_NewState;
      m_BeanIsResting = false;
   }

   if (!m_BeanIsResting && _BeansAreResting())
   {
      m_BeanIsResting = true;
   }
}
