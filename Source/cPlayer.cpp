#include "cPlayer.h"
#include "cBean.h"
#include "cBeanInfo.h"
#include "cFloor.h"
#include "cWall.h"
#include "cRoof.h"
#include "cGameOver.h"

#include "cResources.h"

#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <sstream>
#include <cmath> // ceil
#include <chrono>

cPlayer::cPlayer(
   cResources* a_pResources,
   std::minstd_rand a_RandomNumberEngine,
   std::string a_Identifier
   )
   : cObject(a_pResources),
     m_CurrentState(kStateIdle),
     m_RotationState(kRotationStateUp),
     m_pPivotBean(NULL),
     m_pSwingBean(NULL),
     m_Staging(GetResources(), a_RandomNumberEngine, GetUniqueId()),
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
   SetType(a_Identifier);
   SetSolid(false);

   // Receive the message for when we should start the game
   sMessage l_Request;
   l_Request.m_From = GetResources()->GetMessageDispatcher()->AnyID();
   l_Request.m_Category = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Value = "StartGame";

   std::function<void(sMessage)> l_MessageCallback =
      std::bind(&cPlayer::MessageReceived, this, std::placeholders::_1);

   GetResources()->GetMessageDispatcher()->RegisterForMessages(
      GetUniqueId(),
      l_MessageCallback,
      l_Request
      );
}

cPlayer::~cPlayer()
{
   GetResources()->GetMessageDispatcher()->CancelMessages(GetUniqueId());
}

void cPlayer::Initialize()
{
   cFloor* l_NewFloor = new cFloor(GetResources());
   sf::Vector3<double> l_Position = GetPosition();

   sf::Vector2<uint32_t>* l_pGridCellSize =
      GetResources()->GetActiveLevelData()->GetGridCellSize();

   l_Position.y += l_pGridCellSize->y * 12;
   l_Position.x -= l_pGridCellSize->y;
   l_NewFloor->SetPosition(l_Position, kNormal, false);
   l_NewFloor->Initialize();

   cWall* l_NewWall = new cWall(GetResources());
   l_Position = GetPosition();
   l_Position.x -= l_pGridCellSize->x;
   l_NewWall->SetPosition(l_Position, kNormal, false);
   l_NewWall->Initialize();

   l_NewWall = new cWall(GetResources());
   l_Position = GetPosition();
   l_Position.x += l_pGridCellSize->x * 6;
   l_NewWall->SetPosition(l_Position, kNormal, false);
   l_NewWall->Initialize();

   l_NewWall = new cWall(GetResources());
   l_Position = GetPosition();
   l_Position.x -= l_pGridCellSize->x;
   l_Position.y += l_NewWall->GetBoundingBox().height;
   l_NewWall->SetPosition(l_Position, kNormal, false);
   l_NewWall->Initialize();

   l_NewWall = new cWall(GetResources());
   l_Position = GetPosition();
   l_Position.y += l_NewWall->GetBoundingBox().height;
   l_Position.x += l_pGridCellSize->x * 6;
   l_NewWall->SetPosition(l_Position, kNormal, false);
   l_NewWall->Initialize();

   cRoof * l_NewRoof = new cRoof(GetResources(), GetResources()->GetGameConfigData()->GetProperty(GetType()));
   l_Position = GetPosition();
   l_Position.y -= l_pGridCellSize->y * 4;
   l_Position.x -= l_pGridCellSize->x;
   l_NewRoof->SetPosition(l_Position, kNormal, false);
   l_NewRoof->Initialize();
}

void cPlayer::Event(std::list<sf::Event> * a_pEventList)
{

}

void cPlayer::Step (uint32_t a_ElapsedMiliSec)
{
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

         m_pPivotBean = m_Staging.GetNextBean();
         sf::Vector3<double> l_Position = GetPosition();
         l_Position.x += GetResources()->GetActiveLevelData()->GetGridCellSize()->x * 2;
         l_Position.y -= GetResources()->GetActiveLevelData()->GetGridCellSize()->y;
         m_pPivotBean->SetPosition(l_Position, kNormal, false);

         m_pSwingBean = m_Staging.GetNextBean();
         l_Position = GetPosition();
         l_Position.x += GetResources()->GetActiveLevelData()->GetGridCellSize()->x * 2;
         l_Position.y -= GetResources()->GetActiveLevelData()->GetGridCellSize()->y * 2;
         m_pSwingBean->SetPosition(l_Position, kNormal, false);

         // See if beans are already resting
         if (_BeansAreResting())
         {
            m_BeanIsResting = true;
         }

         m_CurrentState = kStateControlBeans;
         StateChange(kStateCreateBeans, kStateControlBeans);
         m_MiliSecSinceLastFall = 500;

         // Tell the staging observer area to update
         sMessage l_Message;
         l_Message.m_From = GetUniqueId();
         l_Message.m_Category = GetType();
         l_Message.m_Key = GetResources()->GetMessageDispatcher()->Any();
         l_Message.m_Value = "NewBean";
         GetResources()->GetMessageDispatcher()->PostMessage(l_Message);

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

               sf::Vector2<uint32_t>* l_pGridCellSize =
                  GetResources()->GetActiveLevelData()->GetGridCellSize();

               l_RelativePosition.y +=
                  static_cast<double>(l_pGridCellSize->y) / 2;
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
                  //~ std::cout << "Shouldn't ever get here..." << std::endl;
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
               m_CurrentState = kStateCheckForLosingState;
               StateChange(kStateCheckForMatches, kStateCheckForLosingState);
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

         // While garbage accumulator is greater than 6 && row >= 0
         while (m_GarbageAcumulator >= 6 && l_Row >= 0)
         {
            for (uint32_t i = 0; i < 6; ++i)
            {
               if (m_Beans[i][l_Row] == NULL)
               {
                  _CreateGarbageBean(i, l_Row);
                  --m_GarbageAcumulator;
               }
            }

            l_Row -= 1;

         }

         if (l_Row != -1)
         {
            if (m_GarbageAcumulator != 0)
            {
               // create list of numbers 0 - 5
               std::vector<uint32_t> l_Columns;
               for (uint32_t i = 0; i < 6; ++i)
               {
                  l_Columns.push_back(i);
               }

               while (m_GarbageAcumulator != 0)
               {
                  // generate random number between 0 and size
                  //~ std::random_device l_Generator;
                  std::minstd_rand l_Generator(std::chrono::high_resolution_clock::now().time_since_epoch().count());

                  std::uniform_int_distribution<int> l_Distribution(0, l_Columns.size() - 1);
                  int l_Number = l_Distribution(l_Generator);
                  l_Number = l_Distribution(l_Generator);

                  // Pull that number out of the column list
                  uint32_t l_Column = l_Columns[l_Number];
                  l_Columns.erase(l_Columns.begin() + l_Number);
                  if (m_Beans[l_Column][l_Row] == NULL)
                  {
                     _CreateGarbageBean(l_Column, l_Row);
                     --m_GarbageAcumulator;
                  }
                  else if (l_Columns.size() == 0)
                  {
                     // There must have already been garbage in this row
                     // preventing us from unleashing all the garbage we wanted
                     // to.
                     --l_Row;
                     for (uint32_t i = 0; i < 6; ++i)
                     {
                        l_Columns.push_back(i);
                     }
                     if (l_Row < 0)
                     {
                        break;
                     }
                  }
               }

            }
         }

         m_CurrentState = kStateWaitForBeansToSettle;
         StateChange(kStateDropGarbage, kStateWaitForBeansToSettle);
         break;
      }
      case kStateCheckForLosingState:
      {
         // If a bean exists at (2,4) then game over.
         if (m_Beans[2][4] != NULL)
         {
            m_CurrentState = kStateGameLost;
            StateChange(kStateCheckForLosingState, kStateGameLost);
         }
         else
         {
            m_CurrentState = kStateCreateBeans;
            StateChange(kStateCheckForLosingState, kStateCreateBeans);
         }
         break;
      }
      case kStateGameLost:
      {
         sMessage l_Message;
         l_Message.m_From = GetUniqueId();
         l_Message.m_Category = GetResources()->GetMessageDispatcher()->Any();
         l_Message.m_Key = GetResources()->GetMessageDispatcher()->Any();
         l_Message.m_Value = "Player Lost";
         GetResources()->GetMessageDispatcher()->PostMessage(l_Message);

         //~ std::cout << GetType() << " Lost" << std::endl;

         cGameOver* l_pGameOverBanner = new cGameOver(GetResources());
         sf::Vector3<double> l_Position = GetPosition();

         sf::Vector2<uint32_t>* l_pGridCellSize =
            GetResources()->GetActiveLevelData()->GetGridCellSize();

         l_Position.y += l_pGridCellSize->y;
         l_pGameOverBanner->SetPosition(l_Position, kNormal, false);
         l_pGameOverBanner->Initialize();

         m_CurrentState = kStateIdle;
         StateChange(kStateGameLost, kStateIdle);

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
   if (a_Message.m_Value == "StartGame")
   {
      _StartGame();
   }
   else if (a_Message.m_Value == "Player Lost" && a_Message.m_From != GetUniqueId())
   {
      m_CurrentState = kStateIdle;
   }
   else if (a_Message.m_Value == "BeanSettled" && a_Message.m_Category == std::to_string(GetUniqueId()))
   {
      cObject* l_pObject =
         GetResources()->GetActiveLevelData()->GetObjectWithId(a_Message.m_From);
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

   sf::Vector2<uint32_t>* l_pGridCellSize =
      GetResources()->GetActiveLevelData()->GetGridCellSize();

   l_RelativePosition.x +=
      static_cast<double>(l_pGridCellSize->x) * a_RelativeColumn;
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

   sf::Vector2<uint32_t>* l_pGridCellSize =
      GetResources()->GetActiveLevelData()->GetGridCellSize();

   if (  m_RotationState == kRotationStateUp && a_Rotation == kRotateClockwise
      || m_RotationState == kRotationStateDown && a_Rotation == kRotateCounterClockwise
      )
   {
      // Check for collisions to the right of the pivot point
      l_Position.x += l_pGridCellSize->x;
      l_OppositePosition.x -= l_pGridCellSize->x;
      l_NewState = kRotationStateRight;
   }
   else if (  m_RotationState == kRotationStateRight && a_Rotation == kRotateClockwise
           || m_RotationState == kRotationStateLeft && a_Rotation == kRotateCounterClockwise
           )
   {
      // Check for collisions under the pivot point
      l_Position.y += l_pGridCellSize->y;
      l_OppositePosition.y -= l_pGridCellSize->y;
      l_NewState = kRotationStateDown;
   }
   else if (  m_RotationState == kRotationStateDown && a_Rotation == kRotateClockwise
           || m_RotationState == kRotationStateUp && a_Rotation == kRotateCounterClockwise
           )
   {
      // Check for collisions left of the pivot point
      l_Position.x -= l_pGridCellSize->x;
      l_OppositePosition.x += l_pGridCellSize->x;
      l_NewState = kRotationStateLeft;
   }
   else if (  m_RotationState == kRotationStateLeft && a_Rotation == kRotateClockwise
           || m_RotationState == kRotationStateRight && a_Rotation == kRotateCounterClockwise
           )
   {
      // Check for collisions above the pivot point
      l_Position.y -= l_pGridCellSize->y;
      l_OppositePosition.y += l_pGridCellSize->y;
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
      // There is something in the rotation space. If we're trying to rotate up
      // then just quit because we don't want to be pushed down.
      if (  m_RotationState == kRotationStateLeft && a_Rotation == kRotateClockwise
         || m_RotationState == kRotationStateRight && a_Rotation == kRotateCounterClockwise
         )
      {
         return;
      }

      // See if we can scootch over.
      sf::Vector3<double> l_NewSwingPosition = m_pPivotBean->GetPosition();

      if (  m_RotationState == kRotationStateRight && a_Rotation == kRotateClockwise
         || m_RotationState == kRotationStateLeft && a_Rotation == kRotateCounterClockwise
         )
      {
         // If trying to rotate down see if we have to scootch a full space or
         // half space. This allows us to rotate beans under the pivit point
         // nicely when the pivot is half a space from the ground.
         if ((static_cast<uint32_t>(m_pPivotBean->GetPosition().y) % l_pGridCellSize->y) != 0)
         {
            l_OppositePosition.y += l_pGridCellSize->y / 2;
            l_NewSwingPosition.y += l_pGridCellSize->y / 2;
         }
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

std::vector<std::vector<cBeanInfo>> cPlayer::ClonePlayingField()
{
   std::vector<std::vector<cBeanInfo>> l_PlayingField(
      6,
      std::vector<cBeanInfo>(g_kTotalRows)
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
            cBeanInfo l_NewBeanInfo(m_Beans[i][j]->GetColor());
            l_NewBeanInfo.SetGridPosition({i,j});
            l_PlayingField[i][j] = l_NewBeanInfo;

            std::unordered_set<cBean*> l_NewBeanConnections =
               m_Beans[i][j]->GetImmediateConnections();

            for (auto l_Bean : l_NewBeanConnections)
            {
               int32_t l_X =
                  GetBeanGridPosition(l_Bean).x;

               int32_t l_Y =
                  GetBeanGridPosition(l_Bean).y;

               if (l_PlayingField[l_X][l_Y].GetColor() != kBeanColorEmpty)
               {
                  l_PlayingField[l_X][l_Y].AddConnection(&l_PlayingField[i][j]);
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

void cPlayer::_StartGame()
{
   GetResources()->GetMessageDispatcher()->CancelMessages(GetUniqueId());

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

   // Look for messages indicating the game is over
   sMessage l_GameOverRequest;
   l_GameOverRequest.m_From = GetResources()->GetMessageDispatcher()->AnyID();
   l_GameOverRequest.m_Category = GetResources()->GetMessageDispatcher()->Any();
   l_GameOverRequest.m_Key = GetResources()->GetMessageDispatcher()->Any();
   l_GameOverRequest.m_Value = "Player Lost";

   GetResources()->GetMessageDispatcher()->RegisterForMessages(
      GetUniqueId(),
      l_MessageCallback,
      l_GameOverRequest
      );

   m_CurrentState = kStateCreateBeans;
}

bool cPlayer::_BeansAreResting()
{
   sf::Vector2<uint32_t>* l_pGridCellSize =
      GetResources()->GetActiveLevelData()->GetGridCellSize();

   if (  m_RotationState == kRotationStateUp
      || m_RotationState == kRotationStateLeft
      || m_RotationState == kRotationStateRight
      )
   {
      sf::Vector3<double> l_Position = m_pPivotBean->GetPosition();
      l_Position.y += l_pGridCellSize->y / 2;
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
      l_Position.y += l_pGridCellSize->y / 2;
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
   sf::Vector2<uint32_t>* l_pGridCellSize =
      GetResources()->GetActiveLevelData()->GetGridCellSize();

   l_ReturnPosition.x =
      (a_pBean->GetPosition().x - GetPosition().x) / l_pGridCellSize->x;

   // Add 5 because the top 5 above the player is just buffer for garbage. Add .5 so that we round up
   l_ReturnPosition.y =
      ((a_pBean->GetPosition().y - GetPosition().y)/ static_cast<double>(l_pGridCellSize->y))
      + 5.5;

   return l_ReturnPosition;
}

void cPlayer::_CreateGarbageBean(uint32_t a_Column, uint32_t a_Row)
{
   cBean* l_pGarbageBean = new cBean(kBeanColorGarbage, GetResources(), GetUniqueId());
   sf::Vector3<double> l_Position = GetPosition();

   sf::Vector2<uint32_t>* l_pGridCellSize =
      GetResources()->GetActiveLevelData()->GetGridCellSize();

   l_Position.x += l_pGridCellSize->x * a_Column;
   l_Position.y -= l_pGridCellSize->y * (5 - a_Row);
   // Set visible to false because it starts above the playing area.
   // l_pGarbageBean->SetVisible(false);
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

   // Add a little randomness into the garbage. 1 in 5 change to remove one
   //~ std::random_device l_Generator;
   std::minstd_rand l_Generator(std::chrono::high_resolution_clock::now().time_since_epoch().count());

   std::uniform_int_distribution<int> l_Distribution(0, 4);
   int l_Number = l_Distribution(l_Generator);
   l_Number = l_Distribution(l_Generator);
   if (l_Number == 4)
   {
      --l_Garbage;
   }

   return l_Garbage;
}
