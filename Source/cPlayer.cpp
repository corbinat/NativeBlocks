#include "cPlayer.h"
#include "cBean.h"
#include "cBeanInfo.h"
#include "cFloor.h"
#include "cWall.h"
#include "cRoof.h"

#include "cResources.h"

#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <sstream>
#include <cmath> // ceil

cPlayer::cPlayer(
   cResources* a_pResources,
   std::minstd_rand a_RandomNumberEngine
   )
   : cObject(a_pResources),
     m_RandomNumberEngine(),
     m_Initialized(false),
     m_CurrentState(kStateIdle),
     m_RotationState(kRotationStateUp),
     m_pPivotBean(NULL),
     m_pSwingBean(NULL),
     m_FallingBeans(),
     m_NewBeans(),
     m_Beans(6, std::vector<cBean*>(g_kTotalRows, NULL)),
     m_MiliSecPerFall(500),
     m_MiliSecSinceLastFall(0),
     m_RestingBeanTimer(0),
     m_RestingLimit(500),
     m_BeanIsResting(false),
     m_FastFall(false),
     m_TotalSettleTime(0),
     m_MinSettleTime(200),
     m_BeansExploded(0),
     m_ScoreMultiplier(0),
     m_NumberDifferentGroups(0),
     m_ChainCount(0),
     m_GarbageAcumulator(0),
     m_GarbageDropped(false)
{
   SetType("cPlayer");
   SetSolid(false);

   m_RandomNumberEngine = a_RandomNumberEngine;

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

   // Receive messages when garbage beans are sent our way
   sMessage l_GarbageRequest;
   l_GarbageRequest.m_From = GetResources()->GetMessageDispatcher()->AnyID();
   l_GarbageRequest.m_Category = GetResources()->GetMessageDispatcher()->Any();
   l_GarbageRequest.m_Key = "SendingGarbage";
   l_GarbageRequest.m_Value = GetResources()->GetMessageDispatcher()->Any();

   GetResources()->GetMessageDispatcher()->RegisterForMessages(
      GetUniqueId(),
      l_MessageCallback,
      l_GarbageRequest
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
         // Reset Scoring
         m_ChainCount = 0;
         m_GarbageDropped = false;

         // Advance the random number engine
         m_RandomNumberEngine();

         m_pPivotBean = new cBean(m_RandomNumberEngine, GetResources(), GetUniqueId());
         RegisterObject(m_pPivotBean);
         sf::Vector3<double> l_Position = GetPosition();
         l_Position.x += GetResources()->GetGridCellSize().x * 2;
         l_Position.y -= GetResources()->GetGridCellSize().y;
         m_pPivotBean->SetPosition(l_Position, kNormal, false);

         // Advance the random number engine
         m_RandomNumberEngine();

         m_pSwingBean = new cBean(m_RandomNumberEngine, GetResources(),GetUniqueId());
         RegisterObject(m_pSwingBean);
         l_Position = GetPosition();
         l_Position.x += GetResources()->GetGridCellSize().x * 2;
         l_Position.y -= GetResources()->GetGridCellSize().y * 2;
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

         break;
      }

      case kStateWaitForBeansToSettle:
      {
         if (m_FallingBeans.size() == 0)
         {
            m_TotalSettleTime += a_ElapsedMiliSec;
            if (m_TotalSettleTime > m_MinSettleTime)
            {
               m_TotalSettleTime = 0;
               m_CurrentState = kStateCheckForMatches;
               StateChange(kStateWaitForBeansToSettle, kStateCheckForMatches);
            }
         }
         break;
      }

      case kStateCheckForMatches:
      {
         // Find neighbors
         for (auto i = m_NewBeans.begin(); i != m_NewBeans.end(); ++i)
         {
            if ((*i)->GetColor() == kBeanColorGarbage)
            {
               continue;
            }

            double l_X =
               GetBeanGridPosition(*i).x;

            double l_Y =
               GetBeanGridPosition(*i).y;

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
            if (l_Y < g_kTotalRows - 1)
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
               ++m_NumberDifferentGroups;
               uint32_t l_NewBeansExploded = 0;

               std::unordered_set<cBean*> l_ToExplodeList = l_FullConnections;
               for (cBean* l_pConnection : l_FullConnections)
               {
                  uint32_t l_BeanX =
                     GetBeanGridPosition(l_pConnection).x;

                  uint32_t l_BeanY =
                     GetBeanGridPosition(l_pConnection).y;

                  ++m_BeansExploded;
                  ++l_NewBeansExploded;

                  // Find all of the garbage beans touching and add them to the
                  // list of beans to explode
                  if (l_BeanX > 0)
                  {
                     cBean* l_pNeighbor = m_Beans[l_BeanX - 1][l_BeanY];
                     if (l_pNeighbor != NULL && l_pNeighbor->GetColor() == kBeanColorGarbage)
                     {
                        l_ToExplodeList.insert(l_pNeighbor);
                     }
                  }
                  if (l_BeanX < 5)
                  {
                     cBean* l_pNeighbor = m_Beans[l_BeanX + 1][l_BeanY];
                     if (l_pNeighbor != NULL && l_pNeighbor->GetColor() == kBeanColorGarbage)
                     {
                        l_ToExplodeList.insert(l_pNeighbor);
                     }
                  }
                  if (l_BeanY > 0)
                  {
                     cBean* l_pNeighbor = m_Beans[l_BeanX][l_BeanY - 1];
                     if (l_pNeighbor != NULL && l_pNeighbor->GetColor() == kBeanColorGarbage)
                     {
                       l_ToExplodeList.insert(l_pNeighbor);
                     }
                  }
                  if (l_BeanY < g_kTotalRows - 1)
                  {
                     cBean* l_pNeighbor = m_Beans[l_BeanX][l_BeanY + 1];
                     if (l_pNeighbor != NULL && l_pNeighbor->GetColor() == kBeanColorGarbage)
                     {
                        l_ToExplodeList.insert(l_pNeighbor);
                     }
                  }
               }

               if (m_ScoreMultiplier < (l_NewBeansExploded - 3))
               {
                  m_ScoreMultiplier = l_NewBeansExploded - 3;
               }

               for (cBean* l_pConnection : l_ToExplodeList)
               {
                  double l_DeleteX =
                     GetBeanGridPosition(l_pConnection).x;

                  double l_DeleteY =
                     GetBeanGridPosition(l_pConnection).y;

                  m_Beans[l_DeleteX][l_DeleteY] = NULL;

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
                     if (!m_Beans[l_DeleteX][l_DeleteY]->IsExploding())
                     {
                        m_FallingBeans.push_back(m_Beans[l_DeleteX][l_DeleteY]);
                     }
                     m_Beans[l_DeleteX][l_DeleteY] = NULL;
                  }

               }

            }
         }

         // Figure out how many garbage beans to send if some connections were
         // made
         if (m_BeansExploded > 0)
         {
            uint32_t l_TotalToSend =
               _CalculateGarbageBeanNumber();

            sMessage l_Message;
            l_Message.m_From = GetUniqueId();
            l_Message.m_Category = GetResources()->GetMessageDispatcher()->Any();
            l_Message.m_Key = "SendingGarbage";
            l_Message.m_Value = std::to_string(l_TotalToSend);
            GetResources()->GetMessageDispatcher()->PostMessage(l_Message);
            m_CurrentState = kStateWaitForBeansToSettle;
            StateChange(kStateCheckForMatches, kStateWaitForBeansToSettle);
         }

         // Reset temp scores
         m_NumberDifferentGroups = 0;
         m_ScoreMultiplier = 0;
         m_BeansExploded = 0;

         for (auto i = m_FallingBeans.begin(); i != m_FallingBeans.end(); ++i)
         {
            (*i)->Fall();
         }

         m_NewBeans.clear();

         // If the state hasn't changed to wait for beans to settle again, then
         // start next bean drop
         if (m_CurrentState == kStateCheckForMatches)
         {
            if (m_GarbageAcumulator != 0 && !m_GarbageDropped)
            {
               m_CurrentState = kStateDropGarbage;
               StateChange(kStateCheckForMatches, kStateDropGarbage);
            }
            else
            {
               m_CurrentState = kStateCreateBeans;
               StateChange(kStateCheckForMatches, kStateCreateBeans);
            }
         }
         else
         {
            // We made at least one match, so we're rolling back to
            // kStateWaitForBeansToSettle.
            ++m_ChainCount;
         }

         break;
      }
      case kStateDropGarbage:
      {
         m_GarbageDropped = true;
         // Start at row 4. Rows 0 - 4 are for accumulating garbage
         int32_t l_Row = 4;

         uint32_t l_GarbageAcumulator = m_GarbageAcumulator;

         // While garbage accumulator is greater than 6 && row >= 0
         while (l_GarbageAcumulator >= 6 && l_Row >= 0)
         {
            m_GarbageAcumulator -= 6;
            l_GarbageAcumulator -= 6;

            for (uint32_t i = 0; i < 6; ++i)
            {
               _CreateGarbageBean(i, l_Row);
            }

            l_Row -= 1;

         }

         if (l_Row != -1)
         {
            if (l_GarbageAcumulator > 0)
            {
               // create list of numbers 0 - 5
               std::vector<uint32_t> l_Columns;
               for (uint32_t i = 0; i < 6; ++i)
               {
                  l_Columns.push_back(i);
               }

               while (l_GarbageAcumulator > 0)
               {
                  // generate random number between 0 and size
                  std::random_device l_Generator;
                  std::uniform_int_distribution<int> l_Distribution(0, l_Columns.size() - 1);
                  int l_Number = l_Distribution(l_Generator);
                  l_Number = l_Distribution(l_Generator);

                  // Pull that number out of the column list
                  uint32_t l_Column = l_Columns[l_Number];
                  l_Columns.erase(l_Columns.begin() + l_Number);

                  _CreateGarbageBean(l_Column, l_Row);

                  --m_GarbageAcumulator;
                  --l_GarbageAcumulator;
               }

            }
         }

         m_CurrentState = kStateWaitForBeansToSettle;
         StateChange(kStateCheckForMatches, kStateWaitForBeansToSettle);
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
                  GetBeanGridPosition(*i).x;

               double l_Y =
                  GetBeanGridPosition(*i).y;

               m_Beans[l_X][l_Y] = *i;
               m_NewBeans.push_back(*i);
               m_FallingBeans.erase(i);
               return;
            }
         }
      }
   }
   else if(a_Message.m_Key == "SendingGarbage" && a_Message.m_From != GetUniqueId())
   {
      std::istringstream l_Stream(a_Message.m_Value);
      uint32_t l_NewGarbage;
      l_Stream >> l_NewGarbage;

      m_GarbageAcumulator += l_NewGarbage;
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
      if (i != m_pSwingBean && i != m_pPivotBean && i->IsSolid())
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



std::vector<std::vector<std::shared_ptr<cBeanInfo>>> cPlayer::ClonePlayingField()
{
   std::vector<std::vector<std::shared_ptr<cBeanInfo>>> l_PlayingField(
      6,
      std::vector<std::shared_ptr<cBeanInfo>>(g_kTotalRows, NULL)
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
               int32_t l_X =
                  GetBeanGridPosition(l_Bean).x;

               int32_t l_Y =
                  GetBeanGridPosition(l_Bean).y;

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

uint32_t cPlayer::CalculateScore(
   uint32_t a_Matches,
   uint32_t a_Multiplier,
   uint32_t a_Groups,
   uint32_t a_Chains
   )
{
   // Score forumla:
   // (P1X10 + ... + PnX10) x (HighestMultiplier + 3(DifferentGroups-1) + 8*chains)
   uint32_t l_Score = 0;
   uint32_t l_Multiplier = 0;

   // Figure out the multiplier
   if (a_Multiplier > 1)
   {
      l_Multiplier += a_Multiplier;
   }
   l_Multiplier += 3 * (a_Groups - 1);
   l_Multiplier += 8 * a_Chains;
   if (l_Multiplier == 0)
   {
      l_Multiplier = 1;
   }

   // Multiply in the number of beans exploded.
   l_Score = a_Matches * 10;

   return l_Score * l_Multiplier;
}

void cPlayer::_Initialize()
{
   m_Initialized = true;

   cFloor* l_NewFloor = new cFloor(GetResources());
   RegisterObject(l_NewFloor);
   sf::Vector3<double> l_Position = GetPosition();
   l_Position.y += GetResources()->GetGridCellSize().y * 12;
   l_NewFloor->SetPosition(l_Position, kNormal, false);

   cWall* l_NewWall = new cWall(GetResources());
   RegisterObject(l_NewWall);
   l_Position = GetPosition();
   l_Position.x -= GetResources()->GetGridCellSize().x;
   l_Position.y -= GetResources()->GetGridCellSize().y;
   l_NewWall->SetPosition(l_Position, kNormal, false);

   l_NewWall = new cWall(GetResources());
   RegisterObject(l_NewWall);
   l_Position = GetPosition();
   l_Position.x += GetResources()->GetGridCellSize().x * 6;
   l_Position.y -= GetResources()->GetGridCellSize().y;
   l_NewWall->SetPosition(l_Position, kNormal, false);

   l_NewWall = new cWall(GetResources());
   RegisterObject(l_NewWall);
   l_Position = GetPosition();
   l_Position.x -= GetResources()->GetGridCellSize().x;
   l_Position.y += l_NewWall->GetBoundingBox().height - GetResources()->GetGridCellSize().y;
   l_NewWall->SetPosition(l_Position, kNormal, false);

   l_NewWall = new cWall(GetResources());
   RegisterObject(l_NewWall);
   l_Position = GetPosition();
   l_Position.y += l_NewWall->GetBoundingBox().height - GetResources()->GetGridCellSize().y;
   l_Position.x += GetResources()->GetGridCellSize().x * 6;
   l_NewWall->SetPosition(l_Position, kNormal, false);

   cRoof * l_NewRoof = new cRoof(GetResources());
   RegisterObject(l_NewRoof);
   l_Position = GetPosition();
   l_Position.y -= GetResources()->GetGridCellSize().y;
   l_NewRoof->SetPosition(l_Position, kNormal, false);

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
         for (cObject* l_pObject : l_Collisions)
         {
            if (l_pObject->IsSolid())
            {
               return true;
            }
         }
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
         for (cObject* l_pObject : l_Collisions)
         {
            if (l_pObject->IsSolid())
            {
               return true;
            }
         }
      }
   }

   return false;
}

sf::Vector2<uint32_t> cPlayer::GetBeanGridPosition(cBean* a_pBean)
{
   sf::Vector2<uint32_t> l_ReturnPosition;

   l_ReturnPosition.x =
      (a_pBean->GetPosition().x - GetPosition().x) / GetResources()->GetGridCellSize().x;

   // Subtract 5 because the top 5 is above the player is is just buffer for garbage
   l_ReturnPosition.y =
      ((a_pBean->GetPosition().y - GetPosition().y) / GetResources()->GetGridCellSize().y) + 5;

   return l_ReturnPosition;
}

void cPlayer::_CreateGarbageBean(uint32_t a_Column, uint32_t a_Row)
{
   cBean* l_pGarbageBean = new cBean(kBeanColorGarbage, GetResources(), GetUniqueId());
   RegisterObject(l_pGarbageBean);
   sf::Vector3<double> l_Position = GetPosition();
   l_Position.x += GetResources()->GetGridCellSize().x * a_Column;
   l_Position.y -= GetResources()->GetGridCellSize().y * (5 - a_Row);
   l_pGarbageBean->SetPosition(l_Position, kNormal, false);
   l_pGarbageBean->Fall();
   m_FallingBeans.push_back(l_pGarbageBean);
}

uint32_t cPlayer::_CalculateGarbageBeanNumber()
{
   uint32_t l_Score =
      CalculateScore(
         m_BeansExploded,
         m_ScoreMultiplier,
         m_NumberDifferentGroups,
         m_ChainCount
         );

   uint32_t l_Garbage =
      ceil(static_cast<double>(l_Score) / 70.0);
   std::cout << "Sending Garbage: " << l_Garbage << std::endl;

   // Add a little randomness into the garbage. 1 in 5 change to remove one
   std::random_device l_Generator;
   std::uniform_int_distribution<int> l_Distribution(0, 4);
   int l_Number = l_Distribution(l_Generator);
   l_Number = l_Distribution(l_Generator);
   if (l_Number == 4)
   {
      --l_Garbage;
      std::cout << "Actually removing one!" << std::endl;
   }

   return l_Garbage;
}
