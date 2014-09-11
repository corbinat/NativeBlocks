#include "cPlayer.h"
#include "cBean.h"
#include "cBeanInfo.h"
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
     m_TotalSettleTime(0),
     m_MinSettleTime(200),
     m_RoundScore(0)
{
   // Receive messages when beans finish falling. That way we can know when to
   // stop waiting for them to settle.
   sMessage l_Request;
   l_Request.m_From = GetResources()->GetMessageDispatcher()->AnyID();
   l_Request.m_Category = std::to_string(GetUniqueId());
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
         m_pPivotBean = new cBean(GetResources(), GetUniqueId());
         RegisterObject(m_pPivotBean);
         sf::Vector3<double> l_Position = GetPosition();
         l_Position.x += GetResources()->GetGridCellSize().x * 2;
         l_Position.y += GetResources()->GetGridCellSize().y;
         m_pPivotBean->SetPosition(l_Position, kNormal, false);

         m_pSwingBean = new cBean(GetResources(),GetUniqueId());
         RegisterObject(m_pSwingBean);
         l_Position = GetPosition();
         l_Position.x += GetResources()->GetGridCellSize().x * 2;
         m_pSwingBean->SetPosition(l_Position, kNormal, false);

         m_CurrentState = kStateControlBeans;
         StateChange(kStateCreateBeans, kStateControlBeans);

         break;
      }

      case kStateControlBeans:
      {
         if (m_BeanIsResting)
         {
            m_RestingBeanTimer += a_ElapsedMiliSec;

            if (m_RestingBeanTimer > m_RestingLimit || m_FastFall)
            {
               // Clean up and lock in beans
               m_BeanIsResting = false;
               m_RestingBeanTimer = 0;
               m_CurrentState = kStateWaitForBeansToSettle;
               StateChange(kStateControlBeans, kStateWaitForBeansToSettle);
               m_RotationState = kRotationStateUp;
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
               bool l_MoveSuccess = MoveControlledBeans(l_RelativePosition);

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

         ControlBeans(a_ElapsedMiliSec);

         // TODO: Move this stuff to a human player class---------------

         //------------------------------------------------------------------
         break;
      }

      case kStateWaitForBeansToSettle:
      {
         m_TotalSettleTime += a_ElapsedMiliSec;
         if (m_FallingBeans.size() == 0 && m_TotalSettleTime > m_MinSettleTime)
         {
            m_TotalSettleTime = 0;
            m_CurrentState = kStateCheckForMatches;
            StateChange(kStateWaitForBeansToSettle, kStateCheckForMatches);
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

            if (l_FullConnections.size() > 3)
            {
               for (cBean* l_pConnection : l_FullConnections)
               {
                  double l_DeleteX =
                     (l_pConnection->GetPosition().x - GetPosition().x) / GetResources()->GetGridCellSize().x;

                  double l_DeleteY =
                     (l_pConnection->GetPosition().y - GetPosition().y) / GetResources()->GetGridCellSize().y;

                  m_Beans[l_DeleteX][l_DeleteY] = NULL;

                  l_pConnection->Explode();
                  ++m_RoundScore;

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
                     if (!m_Beans[l_DeleteX][l_DeleteY]->IsExploding())
                     {
                        m_FallingBeans.push_back(m_Beans[l_DeleteX][l_DeleteY]);
                     }
                     m_Beans[l_DeleteX][l_DeleteY] = NULL;
                  }

               }
               m_CurrentState = kStateWaitForBeansToSettle;
               StateChange(kStateCheckForMatches, kStateWaitForBeansToSettle);
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
            StateChange(kStateCheckForMatches, kStateCreateBeans);
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
   if (a_Message.m_Value == "BeanSettled" && a_Message.m_Category == std::to_string(GetUniqueId()))
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

sf::Vector2<cBean*> cPlayer::GetBeansInPlay()
{
   sf::Vector2<cBean*> l_Beans;
   l_Beans.x = m_pPivotBean;
   l_Beans.y = m_pSwingBean;

   return l_Beans;
}

bool cPlayer::MoveControlledBeans(sf::Vector3<double> a_NewRelativePosition)
{
   if (m_CurrentState != kStateControlBeans)
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

bool cPlayer::ShiftControlledBeansColumn(int32_t a_RelativeColumn)
{
   sf::Vector3<double> l_RelativePosition;
   l_RelativePosition.x +=
      static_cast<double>(GetResources()->GetGridCellSize().x) * a_RelativeColumn;
   return MoveControlledBeans(l_RelativePosition);
}

void cPlayer::RotateBeans(eRotationDirection a_Rotation)
{
   if (m_CurrentState != kStateControlBeans)
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

eRotationState cPlayer::GetRotationState()
{
   return m_RotationState;
}

void cPlayer::SetFastFall(bool a_FastFall)
{
   m_FastFall = a_FastFall;
}

ePlayerState cPlayer::GetPlayerState()
{
   return m_CurrentState;
}

uint32_t cPlayer::SimulatePlay(
   std::shared_ptr<cBeanInfo> a_pBean1,
   std::shared_ptr<cBeanInfo> a_pBean2,
   std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField
   )
{
   std::cout << "Start Simulating" << std::endl;
   std::cout << "\t" << a_pBean1->GetGridPosition().x << std::endl;

   // Keep track of columns that get modified so that we can check them for
   // matches and things
   std::unordered_set<uint32_t> l_ColumnsOfInterest;

   l_ColumnsOfInterest.insert(a_pBean1->GetGridPosition().x);
   l_ColumnsOfInterest.insert(a_pBean2->GetGridPosition().x);

   a_rPlayingField[a_pBean1->GetGridPosition().x][a_pBean1->GetGridPosition().y] = a_pBean1;
   a_rPlayingField[a_pBean2->GetGridPosition().x][a_pBean2->GetGridPosition().y] = a_pBean2;

   // Search through the columns and explode any beans with 4 connected
   uint32_t l_ReturnScore = 0;

   // _Bubbledown new columns of interest, connect neighbors, and explode again
   while (l_ColumnsOfInterest.size() != 0)
   {
      for (uint32_t l_Column : l_ColumnsOfInterest)
      {
         // Debugging
         std::cout << "Before bubble:";
         for (int i = 0; i < a_rPlayingField[l_Column].size(); ++i)
         {
            if (a_rPlayingField[l_Column][i] == NULL)
            {
               std::cout << "x";
            }
            else if (a_rPlayingField[l_Column][i]->GetGridPosition().y != i)
            {
               std::cout << "!";
            }
            else
            {
               std::cout << "o";
            }
         }
         std::cout << std::endl;

         std::cout << " bubble column: " << l_Column << std::endl;;
         _BubbleBeansDown(a_rPlayingField[l_Column]);

         // Debugging
         std::cout << "After bubble:";
         for (int i = 0; i < a_rPlayingField[l_Column].size(); ++i)
         {
            if (a_rPlayingField[l_Column][i] == NULL)
            {
               std::cout << "x";
            }
            else
            {
               std::cout << "o";
            }
         }
         std::cout << std::endl;

      }
      for (uint32_t l_Column : l_ColumnsOfInterest)
      {
         // Adding to the return score influences the AI to go for connections
         // even if it can't cause beens to explode
         l_ReturnScore +=
            _ConnectColumnNeighbors(l_Column, a_rPlayingField);
      }

      std::unordered_set<uint32_t> l_NewColumnsOfInterest;
      for (uint32_t l_Column : l_ColumnsOfInterest)
      {
         l_ReturnScore +=
            _SearchColumnAndExplodeConnections(
               l_Column,
               a_rPlayingField,
               &l_NewColumnsOfInterest
               );
      }

      l_ColumnsOfInterest = l_NewColumnsOfInterest;

   }
   std::cout << "Done Simulating" << std::endl;
   return l_ReturnScore;
}

std::vector<std::vector<std::shared_ptr<cBeanInfo>>> cPlayer::ClonePlayingField()
{
   std::vector<std::vector<std::shared_ptr<cBeanInfo>>> l_PlayingField(
      6,
      std::vector<std::shared_ptr<cBeanInfo>>(13, NULL)
      );

   for (
      uint32_t i = 0;
      i < m_Beans.size();
      ++i
      )
   {
      for (
         uint32_t j = 0;
         j < m_Beans[i].size();
         ++j
         )
      {
         if (m_Beans[i][j] != NULL)
         {
            std::shared_ptr<cBeanInfo> l_NewBeanInfo =
               std::shared_ptr<cBeanInfo>(new cBeanInfo(m_Beans[i][j]->GetColor()));
            l_NewBeanInfo->SetGridPosition({i,j});
            l_PlayingField[i][j] = l_NewBeanInfo;

            std::unordered_set<cBean*> l_NewBeanConnections =
               m_Beans[i][j]->GetImmediateConnections();

            for (auto l_Bean : l_NewBeanConnections)
            {
               double l_X =
                  (l_Bean->GetPosition().x - GetPosition().x) / GetResources()->GetGridCellSize().x;

               double l_Y =
                  (l_Bean->GetPosition().y - GetPosition().y) / GetResources()->GetGridCellSize().y;

               if (l_PlayingField[l_X][l_Y] != NULL)
               {
                  l_PlayingField[l_X][l_Y]->AddConnection(l_NewBeanInfo.get());
               }
            }

         }
      }

   }

   return l_PlayingField;
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

sf::Vector2<uint32_t> cPlayer::GetBeanGridPosition(cBean* a_pBean)
{
   sf::Vector2<uint32_t> l_ReturnPosition;

   l_ReturnPosition.x =
      (a_pBean->GetPosition().x - GetPosition().x) / GetResources()->GetGridCellSize().x;

   l_ReturnPosition.y =
      (a_pBean->GetPosition().y - GetPosition().y) / GetResources()->GetGridCellSize().y;

   return l_ReturnPosition;
}

uint32_t cPlayer::_ConnectBeanToNeighbors(
   std::shared_ptr<cBeanInfo> a_pBean,
   std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField
   )
{
   uint32_t l_ReturnConnections = 0;

   if (a_pBean->GetColor() == kBeanColorGarbage)
   {
      return l_ReturnConnections;
   }

   uint32_t l_X = a_pBean->GetGridPosition().x;
   uint32_t l_Y = a_pBean->GetGridPosition().y;
   if (l_X > 0)
   {
      std::shared_ptr<cBeanInfo> l_pNeighbor = a_rPlayingField[l_X - 1][l_Y];
      if (l_pNeighbor != NULL && l_pNeighbor->GetColor() == a_pBean->GetColor())
      {
         if (a_pBean->AddConnection(l_pNeighbor.get()))
         {
            ++l_ReturnConnections;
         }
      }
   }
   if (l_X < 5)
   {
      std::shared_ptr<cBeanInfo> l_pNeighbor = a_rPlayingField[l_X + 1][l_Y];
      if (l_pNeighbor != NULL && l_pNeighbor->GetColor() == a_pBean->GetColor())
      {
         if (a_pBean->AddConnection(l_pNeighbor.get()))
         {
            ++l_ReturnConnections;
         }
      }
   }
   if (l_Y > 0)
   {
      std::shared_ptr<cBeanInfo> l_pNeighbor = a_rPlayingField[l_X][l_Y - 1];
      if (l_pNeighbor != NULL && l_pNeighbor->GetColor() == a_pBean->GetColor())
      {
         if (a_pBean->AddConnection(l_pNeighbor.get()))
         {
            ++l_ReturnConnections;
         }
      }
   }
   if (l_Y < 12)
   {
      std::shared_ptr<cBeanInfo> l_pNeighbor = a_rPlayingField[l_X][l_Y + 1];
      if (l_pNeighbor != NULL && l_pNeighbor->GetColor() == a_pBean->GetColor())
      {
         if (a_pBean->AddConnection(l_pNeighbor.get()))
         {
            ++l_ReturnConnections;
         }
      }
   }

   return l_ReturnConnections;
}

uint32_t cPlayer::_ConnectColumnNeighbors(
   uint32_t a_Column,
   std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField
   )
{
   uint32_t l_ReturnConnections = 0;

   for (uint32_t l_Row = 0; l_Row < a_rPlayingField[a_Column].size(); ++l_Row)
   {
      if (a_rPlayingField[a_Column][l_Row] != NULL)
      {
         l_ReturnConnections +=
            _ConnectBeanToNeighbors(
               a_rPlayingField[a_Column][l_Row],
               a_rPlayingField
               );
      }
   }

   return l_ReturnConnections;
}

bool cPlayer::_BubbleBeansDown(std::vector<std::shared_ptr<cBeanInfo>>& a_rColumn)
{
   std::cout << "Bubbling Down" << std::endl;
   bool l_SawNull = false;
   bool l_RowUpdated = false;
   uint32_t l_FirstNull = a_rColumn.size() - 1;
   for (
      int32_t i = a_rColumn.size() - 1;
      i >= 0;
      --i
      )
   {
      if (a_rColumn[i] == NULL)
      {
         if (l_SawNull == false && i < l_FirstNull)
         {
            l_FirstNull = i;
         }
         l_SawNull = true;
      }
      else if (l_SawNull)
      {
         std::cout << "Moving " << i << " to " << i+1 << std::endl;
         std::shared_ptr<cBeanInfo> l_Element = a_rColumn[i];
         std::cout << "Removing connections" << std::endl;
         l_Element->RemoveAllConnections();
         std::cout << "Performing Swap" << std::endl;
         a_rColumn[i] = a_rColumn[i + 1];
         a_rColumn[i + 1] = l_Element;
         l_Element->SetRowPosition(i + 1);
         std::cout << "Done with swap" << std::endl;
         l_SawNull = false;
         l_RowUpdated = true;

         // Add one because the next loop will decrement it again
         i = l_FirstNull + 1;
      }
   }
   std::cout << "Done Bubbling Down" << std::endl;
   return l_RowUpdated;
}

uint32_t cPlayer::_SearchColumnAndExplodeConnections(
   uint32_t a_Column,
   std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField,
   std::unordered_set<uint32_t>* a_pNewColumnsOfInterest
   )
{
   std::cout << "Searching columns" << std::endl;
   uint32_t l_ReturnScore = 0;
   for (uint32_t l_Row = 0; l_Row < a_rPlayingField[a_Column].size(); ++l_Row)
   {
      if (a_rPlayingField[a_Column][l_Row] != NULL)
      {
         std::unordered_set<cBeanInfo*> l_Connections =
            a_rPlayingField[a_Column][l_Row]->CountConnections();

         std::cout << "Connections at " << a_Column << "," << l_Row << ": " << l_Connections.size() << std::endl;
         std::cout << "\tShould be same: " << a_rPlayingField[a_Column][l_Row]->GetGridPosition().x << "," << a_rPlayingField[a_Column][l_Row]->GetGridPosition().y << std::endl;

         if (l_Connections.size() > 3)
         {
            std::cout << "Exploding:";
            // Explode this bean and all of the connected beans
            for (cBeanInfo* l_pConnection : l_Connections)
            {
               uint32_t l_DeleteX = l_pConnection->GetGridPosition().x;
               uint32_t l_DeleteY = l_pConnection->GetGridPosition().y;
               std::cout << "(" << l_DeleteX << "," << l_DeleteY << ") ";

               a_rPlayingField[l_DeleteX][l_DeleteY] = NULL;
               l_pConnection->RemoveAllConnections();
               a_pNewColumnsOfInterest->insert(l_DeleteX);
               ++l_ReturnScore;
            }
            std::cout << std::endl;

               // Debugging
            std::cout << "After exploding " << a_Column << ":";
            for (int i = 0; i < a_rPlayingField[a_Column].size(); ++i)
            {
               if (a_rPlayingField[a_Column][i] == NULL)
               {
                  std::cout << "x";
               }
               else
               {
                  std::cout << "o";
               }
            }
            std::cout << std::endl;
         }
      }
   }
   std::cout << "Done Searching columns" << std::endl;
   return l_ReturnScore;
}
