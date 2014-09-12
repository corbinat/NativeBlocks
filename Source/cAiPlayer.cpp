
#include "cAiPlayer.h"
#include "cBean.h"
#include "cBeanInfo.h"

#include <iostream>

cAiPlayer::cAiPlayer(cResources* a_pResources)
   : cPlayer(a_pResources),
     m_OptimalMoves(),
     m_DoneThinking(false)
{

}

cAiPlayer::~cAiPlayer()
{
}

void cAiPlayer::StateChange(ePlayerState a_Old, ePlayerState a_New)
{
   // Once we enter the kStateControlBeans state the AI needs to start thinking
   // about where to put the beans
   if (a_New == kStateControlBeans)
   {
      // Start simulating
      std::vector<std::vector<std::shared_ptr<cBeanInfo>>> l_PlayingField =
         ClonePlayingField();

      // Copy the current beans in play
      sf::Vector2<cBean*> l_BeansInPlay = GetBeansInPlay();

      std::shared_ptr<cBeanInfo> l_PivotBean =
         std::shared_ptr<cBeanInfo>(new cBeanInfo(l_BeansInPlay.x->GetColor()));
      std::shared_ptr<cBeanInfo> l_SwingBean =
         std::shared_ptr<cBeanInfo>(new cBeanInfo(l_BeansInPlay.y->GetColor()));

      sf::Vector2<uint32_t> l_PivotPosition =
         GetBeanGridPosition(l_BeansInPlay.x);
      sf::Vector2<uint32_t> l_SwingPosition =
         GetBeanGridPosition(l_BeansInPlay.y);

      std::vector<eRotationState> l_RotationStates;
      l_RotationStates.push_back(kRotationStateUp);
      l_RotationStates.push_back(kRotationStateDown);
      l_RotationStates.push_back(kRotationStateLeft);
      l_RotationStates.push_back(kRotationStateRight);

      for (eRotationState l_RotationState : l_RotationStates)
      {
         l_PivotBean->SetGridPosition(l_PivotPosition);
         l_SwingBean->SetGridPosition(l_SwingPosition);
         // Adjust the swing bean for each rotation state
         uint32_t l_MinRow = 0;
         uint32_t l_MaxRow = 5;

         if (l_RotationState == kRotationStateLeft)
         {
            l_MinRow = 1;
            l_SwingBean->SetColumnPosition(l_SwingBean->GetGridPosition().x - 1);
         }
         else if (l_RotationState == kRotationStateRight)
         {
            l_MaxRow = 4;
            l_SwingBean->SetColumnPosition(l_SwingBean->GetGridPosition().x + 1);
         }
         else if (l_RotationState == kRotationStateDown)
         {
            l_SwingBean->SetRowPosition(l_SwingBean->GetGridPosition().y + 2);
         }

         std::vector<std::vector<std::shared_ptr<cBeanInfo>>> l_CopyPlayingField =
            DeepCopyGivenPlayingField(l_PlayingField);
         _AnalyzeMove(l_PivotBean, l_SwingBean, l_RotationState, l_CopyPlayingField);

         // Simulate to the left.
         // TODO: This allows us to stop if we hit a wall of beans
         while (l_PivotBean->GetGridPosition().x > l_MinRow)
         {
            l_PivotBean->SetColumnPosition(l_PivotBean->GetGridPosition().x - 1);
            l_SwingBean->SetColumnPosition(l_SwingBean->GetGridPosition().x - 1);
            std::vector<std::vector<std::shared_ptr<cBeanInfo>>> l_CopyPlayingField =
               DeepCopyGivenPlayingField(l_PlayingField);
            _AnalyzeMove(l_PivotBean, l_SwingBean, l_RotationState, l_CopyPlayingField);
         }

         l_PivotBean->SetGridPosition(l_PivotPosition);
         l_SwingBean->SetGridPosition(l_SwingPosition);
         if (l_RotationState == kRotationStateLeft)
         {
            l_MinRow = 1;
            l_SwingBean->SetColumnPosition(l_SwingBean->GetGridPosition().x - 1);
         }
         else if (l_RotationState == kRotationStateRight)
         {
            l_MaxRow = 4;
            l_SwingBean->SetColumnPosition(l_SwingBean->GetGridPosition().x + 1);
         }
         else if (l_RotationState == kRotationStateDown)
         {
            l_SwingBean->SetRowPosition(l_SwingBean->GetGridPosition().y + 2);
         }

         // Simulate to the right
         while (l_PivotBean->GetGridPosition().x < l_MaxRow)
         {
            l_PivotBean->SetColumnPosition(l_PivotBean->GetGridPosition().x + 1);
            l_SwingBean->SetColumnPosition(l_SwingBean->GetGridPosition().x + 1);
            std::vector<std::vector<std::shared_ptr<cBeanInfo>>> l_CopyPlayingField =
               DeepCopyGivenPlayingField(l_PlayingField);
            _AnalyzeMove(l_PivotBean, l_SwingBean, l_RotationState, l_CopyPlayingField);
         }
      }

      // We now have a list of potential moves. Pick one at random to work
      // towards.
      std::random_device l_Generator;
      std::uniform_int_distribution<int> l_Distribution(0, m_OptimalMoves.size() - 1);

      int l_Number = l_Distribution(l_Generator);
      l_Number = l_Distribution(l_Generator);

      sOptimalPosition l_Final = m_OptimalMoves[l_Number];
      m_OptimalMoves.clear();
      m_OptimalMoves.push_back(l_Final);
      m_DoneThinking = true;
   }
   else
   {
      m_OptimalMoves.clear();
      m_DoneThinking = false;
   }
}

void cAiPlayer::ControlBeans(uint32_t a_ElapsedMiliSec)
{
   // Move to where we settle
   if (!m_DoneThinking)
   {
      return;
   }

   // There should only be one move in the vector. Pull it out and move towards
   // it.
   sOptimalPosition l_Destination = m_OptimalMoves[0];

   // If our rotation isn't right, rotate the beans.
   if (GetRotationState() != l_Destination.m_Rotation)
   {
      RotateBeans(kRotateClockwise);
      return;
   }

   // Get the first bean in play (The pivot bean)
   sf::Vector2<cBean*> l_BeansInPlay = GetBeansInPlay();

   // Get the pivot bean's location
   sf::Vector2<uint32_t> l_BeanPos = GetBeanGridPosition(l_BeansInPlay.x);


   if (l_BeanPos.x < l_Destination.m_Column)
   {
      ShiftControlledBeansColumn(1);
   }
   else if (l_BeanPos.x > l_Destination.m_Column)
   {
      ShiftControlledBeansColumn(-1);
   }
   else
   {
      // The rotation and position are good, so just fall
      SetFastFall(true);
   }

}

std::vector<std::vector<std::shared_ptr<cBeanInfo>>> cAiPlayer::DeepCopyGivenPlayingField(
   std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField
   )
{
    std::vector<std::vector<std::shared_ptr<cBeanInfo>>> l_PlayingField(
      6,
      std::vector<std::shared_ptr<cBeanInfo>>(g_kTotalRows, NULL)
      );

   for (
      uint32_t i = 0;
      i < a_rPlayingField.size();
      ++i
      )
   {
      for (
         uint32_t j = 0;
         j < a_rPlayingField[i].size();
         ++j
         )
      {
         if (a_rPlayingField[i][j] != NULL)
         {
            std::shared_ptr<cBeanInfo> l_NewBeanInfo =
               std::shared_ptr<cBeanInfo>(new cBeanInfo(a_rPlayingField[i][j]->GetColor()));
            l_NewBeanInfo->SetGridPosition({i,j});
            l_PlayingField[i][j] = l_NewBeanInfo;

            std::unordered_set<cBeanInfo*> l_NewBeanConnections =
               a_rPlayingField[i][j]->GetImmediateConnections();

            for (auto l_Bean : l_NewBeanConnections)
            {
               double l_X = l_Bean->GetGridPosition().x;
               double l_Y = l_Bean->GetGridPosition().y;

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

void cAiPlayer::_AnalyzeMove(
   std::shared_ptr<cBeanInfo> a_pBean1,
   std::shared_ptr<cBeanInfo> a_pBean2,
   eRotationState a_RotationState,
   std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField
   )
{
   sf::Vector2<uint32_t> l_PivotPosition = a_pBean1->GetGridPosition();
   sf::Vector2<uint32_t> l_SwingPosition = a_pBean2->GetGridPosition();

   sOptimalPosition l_NewMove;
   l_NewMove.m_Column = a_pBean1->GetGridPosition().x;
   l_NewMove.m_Rotation = a_RotationState;
   l_NewMove.m_Score =
      _SimulatePlay(a_pBean1, a_pBean2, a_rPlayingField);

   if (m_OptimalMoves.size() == 0)
   {
      m_OptimalMoves.push_back(l_NewMove);
   }
   else if (m_OptimalMoves.begin()->m_Score == l_NewMove.m_Score)
   {
      m_OptimalMoves.push_back(l_NewMove);
   }
   else if (m_OptimalMoves.begin()->m_Score < l_NewMove.m_Score)
   {
      m_OptimalMoves.clear();
      m_OptimalMoves.push_back(l_NewMove);
   }

   // SimulatePlay dirtys up the Beans. Restore the bean's position and
   // connectins for the next simulation.
   a_pBean1->SetGridPosition(l_PivotPosition);
   a_pBean2->SetGridPosition(l_SwingPosition);

   a_pBean1->RemoveAllConnections();
   a_pBean2->RemoveAllConnections();
}

uint32_t cAiPlayer::_SimulatePlay(
   std::shared_ptr<cBeanInfo> a_pBean1,
   std::shared_ptr<cBeanInfo> a_pBean2,
   std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField
   )
{
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
         _BubbleBeansDown(a_rPlayingField[l_Column]);

      }
      for (uint32_t l_Column : l_ColumnsOfInterest)
      {
         // Adding to the return score influences the AI to go for connections
         // even if it can't cause beens to explode
         //l_ReturnScore +=
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
   return l_ReturnScore;
}

uint32_t cAiPlayer::_ConnectBeanToNeighbors(
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
   if (l_Y < g_kTotalRows - 1)
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

uint32_t cAiPlayer::_ConnectColumnNeighbors(
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

bool cAiPlayer::_BubbleBeansDown(std::vector<std::shared_ptr<cBeanInfo>>& a_rColumn)
{
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
         std::shared_ptr<cBeanInfo> l_Element = a_rColumn[i];
         l_Element->RemoveAllConnections();
         a_rColumn[i] = a_rColumn[i + 1];
         a_rColumn[i + 1] = l_Element;
         l_Element->SetRowPosition(i + 1);
         l_SawNull = false;
         l_RowUpdated = true;

         // Add one because the next loop will decrement it again
         i = l_FirstNull + 1;
      }
   }
   return l_RowUpdated;
}

uint32_t cAiPlayer::_SearchColumnAndExplodeConnections(
   uint32_t a_Column,
   std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField,
   std::unordered_set<uint32_t>* a_pNewColumnsOfInterest
   )
{
   uint32_t l_ReturnScore = 0;
   for (uint32_t l_Row = 0; l_Row < a_rPlayingField[a_Column].size(); ++l_Row)
   {
      if (a_rPlayingField[a_Column][l_Row] != NULL)
      {
         std::unordered_set<cBeanInfo*> l_Connections =
            a_rPlayingField[a_Column][l_Row]->CountConnections();


         if (l_Connections.size() > 3)
         {
            // Explode this bean and all of the connected beans
            for (cBeanInfo* l_pConnection : l_Connections)
            {
               uint32_t l_DeleteX = l_pConnection->GetGridPosition().x;
               uint32_t l_DeleteY = l_pConnection->GetGridPosition().y;

               a_rPlayingField[l_DeleteX][l_DeleteY] = NULL;
               l_pConnection->RemoveAllConnections();
               a_pNewColumnsOfInterest->insert(l_DeleteX);
               ++l_ReturnScore;


               // Delete garbage beans that are touching
               if (l_DeleteX > 0)
               {
                  std::shared_ptr<cBeanInfo> l_pNeighbor = a_rPlayingField[l_DeleteX - 1][l_DeleteY];
                  if (l_pNeighbor != NULL && l_pNeighbor->GetColor() == kBeanColorGarbage)
                  {
                     a_rPlayingField[l_DeleteX - 1][l_DeleteY] = NULL;
                     a_pNewColumnsOfInterest->insert(l_DeleteX - 1);
                  }
               }
               if (l_DeleteX < 5)
               {
                  std::shared_ptr<cBeanInfo> l_pNeighbor = a_rPlayingField[l_DeleteX + 1][l_DeleteY];
                  if (l_pNeighbor != NULL && l_pNeighbor->GetColor() == kBeanColorGarbage)
                  {
                     a_rPlayingField[l_DeleteX + 1][l_DeleteY] = NULL;
                     a_pNewColumnsOfInterest->insert(l_DeleteX + 1);
                  }
               }
               if (l_DeleteY > 0)
               {
                  std::shared_ptr<cBeanInfo> l_pNeighbor = a_rPlayingField[l_DeleteX][l_DeleteY - 1];
                  if (l_pNeighbor != NULL && l_pNeighbor->GetColor() == kBeanColorGarbage)
                  {
                     a_rPlayingField[l_DeleteX][l_DeleteY - 1] = NULL;
                  }
               }
               if (l_DeleteY < g_kTotalRows - 1)
               {
                  std::shared_ptr<cBeanInfo> l_pNeighbor = a_rPlayingField[l_DeleteX][l_DeleteY + 1];
                  if (l_pNeighbor != NULL && l_pNeighbor->GetColor() == kBeanColorGarbage)
                  {
                     a_rPlayingField[l_DeleteX][l_DeleteY + 1] = NULL;
                  }
               }
            }
         }
      }
   }
   return l_ReturnScore;
}

