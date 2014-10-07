
#include "cAiPlayer.h"
#include "cBean.h"
#include "cBeanInfo.h"

#include <iostream>

cAiPlayer::cAiPlayer(cResources* a_pResources, std::minstd_rand a_RandomNumberEngine, std::string a_Identifier)
   : cPlayer(a_pResources, a_RandomNumberEngine, a_Identifier),
     m_OptimalMoves(),
     m_DoneThinking(false),
     m_DelayToFirstMove(0),
     m_DelayTimer(0),
     m_AIThoughtLevel(1)
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

      // TODO: This is a little weird because the l_Positions is only used as
      // internal book-keeping during the function's recursion. I could make a
      // small wrapper to hide this detail.
      sOptimalPosition l_Positions;
      _AnalyzeAllMoves(l_PlayingField, 0, l_Positions);
   }
   else
   {
      m_OptimalMoves.clear();
      m_DoneThinking = false;
      SetFastFall(false);
   }
}

void cAiPlayer::ControlBeans(uint32_t a_ElapsedMiliSec)
{
   // Move to where we settle
   if (!m_DoneThinking)
   {
      return;
   }

   //~ m_DelayTimer += a_ElapsedMiliSec;
   //~ if (m_DelayTimer < m_DelayToFirstMove)
   //~ {
      //~ return;
   //~ }

   //m_DelayTimer = m_DelayToFirstMove - 100;

   // There should only be one move in the vector. Pull it out and move towards
   // it.
   sOptimalPosition l_Destination;
   l_Destination.m_Column = 2;
   l_Destination.m_Rotation = kRotationStateUp;
   if (m_OptimalMoves.size() != 0)
   {
      l_Destination = m_OptimalMoves[0];
   }

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

   bool l_Success = true;

   if (l_BeanPos.x < l_Destination.m_Column)
   {
      l_Success = ShiftControlledBeansColumn(1);
   }
   else if (l_BeanPos.x > l_Destination.m_Column)
   {
      l_Success = ShiftControlledBeansColumn(-1);
   }
   else
   {
      // The rotation and position are good, so just fall
      SetFastFall(true);
   }

   if (!l_Success)
   {
      m_OptimalMoves.clear();
      std::cout << "AHHHHH START OVER" << l_Destination.m_Column << " " << l_Destination.m_Rotation << std::endl;
      // AI can't go where it wants because wall of beans. Start thinking again
      std::vector<std::vector<std::shared_ptr<cBeanInfo>>> l_PlayingField =
         ClonePlayingField();

      // TODO: This is a little weird because the l_Positions is only used as
      // internal book-keeping during the function's recursion. I could make a
      // small wrapper to hide this detail.
      sOptimalPosition l_Positions;
      _AnalyzeAllMoves(l_PlayingField, 0, l_Positions);
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

void cAiPlayer::_AnalyzeAllMoves(
   std::vector<std::vector<std::shared_ptr<cBeanInfo>>> & a_PlayingField,
   uint32_t a_Depth,
   sOptimalPosition a_InitialMove
   )
{
   m_DoneThinking = false;

   if (a_Depth > m_AIThoughtLevel)
   {
      return;
   }

   std::shared_ptr<cBeanInfo> l_PivotBean;
   sf::Vector2<uint32_t> l_PivotPosition;
   std::shared_ptr<cBeanInfo> l_SwingBean;
   sf::Vector2<uint32_t> l_SwingPosition;

   if (a_Depth == 0)
   {
       // Copy the current beans in play
      sf::Vector2<cBean*> l_BeansInPlay = GetBeansInPlay();

      l_PivotBean =
         std::shared_ptr<cBeanInfo>(new cBeanInfo(l_BeansInPlay.x->GetColor()));
      l_SwingBean =
         std::shared_ptr<cBeanInfo>(new cBeanInfo(l_BeansInPlay.y->GetColor()));

      l_PivotPosition =
         GetBeanGridPosition(l_BeansInPlay.x);

      // Start the beans in the up position
      l_SwingPosition.x = l_PivotPosition.x;
      l_SwingPosition.y = l_PivotPosition.y - 1;
   }
   else
   {
      eBeanColor l_Color = m_Staging.InspectNextBeanColor(a_Depth * 2 - 2);
      eBeanColor l_Color2 = m_Staging.InspectNextBeanColor(a_Depth * 2 + 1 - 2);
      //std::cout << "AI color: " << l_Color << "," << l_Color2 << std::endl;

      l_PivotBean =
         std::shared_ptr<cBeanInfo>(new cBeanInfo(l_Color));
      l_SwingBean =
         std::shared_ptr<cBeanInfo>(new cBeanInfo(l_Color2));

      l_PivotPosition.x = 2;
      l_PivotPosition.y = 4;

      l_SwingPosition.x = 2;
      l_SwingPosition.y = 3;
   }

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
         if (l_SwingBean->GetGridPosition().x > l_MinRow && a_PlayingField[l_SwingBean->GetGridPosition().x - 1][l_SwingBean->GetGridPosition().y] == NULL)
         {
            l_SwingBean->SetColumnPosition(l_SwingBean->GetGridPosition().x - 1);
         }
         else
         {
            // we can't rotate left easily.
            continue;
         }

         l_MinRow = 1;
      }
      else if (l_RotationState == kRotationStateRight)
      {
         if (l_SwingBean->GetGridPosition().x < l_MaxRow && a_PlayingField[l_SwingBean->GetGridPosition().x + 1][l_SwingBean->GetGridPosition().y] == NULL)
         {
            l_SwingBean->SetColumnPosition(l_SwingBean->GetGridPosition().x + 1);
         }
         else
         {
            // we can't rotate right easily.
            continue;
         }
         l_MaxRow = 4;

      }
      else if (l_RotationState == kRotationStateDown)
      {
         if (l_PivotBean->GetColor() == l_SwingBean->GetColor())
         {
            // This is same state as up if both beans have same color.
            continue;
         }
         if (a_PlayingField[l_SwingBean->GetGridPosition().x][l_SwingBean->GetGridPosition().y + 1] == NULL)
         {
            l_SwingBean->SetRowPosition(l_SwingBean->GetGridPosition().y + 2);
         }
         else
         {
            // we can't rotate down easily.
            continue;
         }
      }

      std::vector<std::vector<std::shared_ptr<cBeanInfo>>> l_CopyPlayingField =
         DeepCopyGivenPlayingField(a_PlayingField);
      _AnalyzeMove(
         l_PivotBean,
         l_SwingBean,
         l_RotationState,
         l_CopyPlayingField,
         a_InitialMove,
         a_Depth
         );

      // Simulate to the left.
      while (l_PivotBean->GetGridPosition().x > l_MinRow)
      {
         std::vector<std::vector<std::shared_ptr<cBeanInfo>>> l_CopyPlayingField =
            DeepCopyGivenPlayingField(a_PlayingField);

         // See if we can even go left
         if (  l_CopyPlayingField[l_PivotBean->GetGridPosition().x - 1][l_PivotBean->GetGridPosition().y] != NULL
            || l_CopyPlayingField[l_SwingBean->GetGridPosition().x - 1][l_SwingBean->GetGridPosition().y] != NULL
            )
         {
            break;
         }

         l_PivotBean->SetColumnPosition(l_PivotBean->GetGridPosition().x - 1);
         l_SwingBean->SetColumnPosition(l_SwingBean->GetGridPosition().x - 1);

         _AnalyzeMove(
            l_PivotBean,
            l_SwingBean,
            l_RotationState,
            l_CopyPlayingField,
            a_InitialMove,
            a_Depth
            );
      }

      // Reset to middle to prepare for going right
      l_PivotBean->SetGridPosition(l_PivotPosition);
      l_SwingBean->SetGridPosition(l_SwingPosition);
      if (l_RotationState == kRotationStateLeft)
      {
         if (l_SwingBean->GetGridPosition().x > l_MinRow && a_PlayingField[l_SwingBean->GetGridPosition().x - 1][l_SwingBean->GetGridPosition().y] == NULL)
         {
            l_SwingBean->SetColumnPosition(l_SwingBean->GetGridPosition().x - 1);
         }
         else
         {
            // we can't rotate left easily.
            continue;
         }

         l_MinRow = 1;
      }
      else if (l_RotationState == kRotationStateRight)
      {
         if (l_SwingBean->GetGridPosition().x < l_MaxRow && a_PlayingField[l_SwingBean->GetGridPosition().x + 1][l_SwingBean->GetGridPosition().y] == NULL)
         {
            l_SwingBean->SetColumnPosition(l_SwingBean->GetGridPosition().x + 1);
         }
         else
         {
            // we can't rotate right easily.
            continue;
         }
         l_MaxRow = 4;

      }
      else if (l_RotationState == kRotationStateDown)
      {
         if (l_PivotBean->GetColor() == l_SwingBean->GetColor())
         {
            // This is same state as up if both beans have same color.
            continue;
         }
         if (a_PlayingField[l_SwingBean->GetGridPosition().x][l_SwingBean->GetGridPosition().y + 1] == NULL)
         {
            l_SwingBean->SetRowPosition(l_SwingBean->GetGridPosition().y + 2);
         }
         else
         {
            // we can't rotate down easily.
            continue;
         }
      }

      // Simulate to the right
      while (l_PivotBean->GetGridPosition().x < l_MaxRow)
      {
         std::vector<std::vector<std::shared_ptr<cBeanInfo>>> l_CopyPlayingField =
            DeepCopyGivenPlayingField(a_PlayingField);

         // See if we can even go right
         if (  l_CopyPlayingField[l_PivotBean->GetGridPosition().x + 1][l_PivotBean->GetGridPosition().y] != NULL
            || l_CopyPlayingField[l_SwingBean->GetGridPosition().x + 1][l_SwingBean->GetGridPosition().y] != NULL
            )
         {
            break;
         }

         l_PivotBean->SetColumnPosition(l_PivotBean->GetGridPosition().x + 1);
         l_SwingBean->SetColumnPosition(l_SwingBean->GetGridPosition().x + 1);

         _AnalyzeMove(
            l_PivotBean,
            l_SwingBean,
            l_RotationState,
            l_CopyPlayingField,
            a_InitialMove,
            a_Depth
            );
      }
   }

   if (a_Depth == 0)
   {
      // We now have a list of potential moves. Pick one at random to work
      // towards.
      if (m_OptimalMoves.size() != 0)
      {
         std::random_device l_Generator;
         std::uniform_int_distribution<int> l_Distribution(0, m_OptimalMoves.size() - 1);

         int l_Number = l_Distribution(l_Generator);
         l_Number = l_Distribution(l_Generator);

         sOptimalPosition l_Final = m_OptimalMoves[l_Number];
         m_OptimalMoves.clear();
         m_OptimalMoves.push_back(l_Final);

         if (GetType() == "Player1")
         {
            std::cout << "Expecting Score: " << l_Final.m_Score << std::endl;
            std::cout << l_Final.m_Column << " " << l_Final.m_Rotation << std::endl;
         }
      }

      m_DoneThinking = true;
      //~ std::uniform_int_distribution<int> l_Distribution2(200, 2000);
      //~ m_DelayToFirstMove = l_Distribution2(l_Generator);
      //~ m_DelayToFirstMove = l_Distribution2(l_Generator);
      //~ m_DelayTimer = 0;
   }
}

void cAiPlayer::_AnalyzeMove(
   std::shared_ptr<cBeanInfo> a_pBean1,
   std::shared_ptr<cBeanInfo> a_pBean2,
   eRotationState a_RotationState,
   std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField,
   sOptimalPosition a_InitialMove,
   uint32_t a_Depth
   )
{
   sf::Vector2<uint32_t> l_PivotPosition = a_pBean1->GetGridPosition();
   sf::Vector2<uint32_t> l_SwingPosition = a_pBean2->GetGridPosition();

   if (a_Depth == 0)
   {
      a_InitialMove.m_Column = a_pBean1->GetGridPosition().x;
      a_InitialMove.m_Rotation = a_RotationState;
   }

   a_InitialMove.m_Score =
      _SimulatePlay(a_pBean1, a_pBean2, a_rPlayingField) + a_InitialMove.m_Score;

   if (a_InitialMove.m_Score != 0 && a_Depth == 0)
   {
      // Add a bonus to doing things ealier
      a_InitialMove.m_Score *= 2;
   }

   // See if we have lost. If so, don't bother continuing on.
   if (a_rPlayingField[2][5] != NULL)
   {
      // SimulatePlay dirtys up the Beans. Restore the bean's position and
      // connections for the next simulation.
      a_pBean1->SetGridPosition(l_PivotPosition);
      a_pBean2->SetGridPosition(l_SwingPosition);

      a_pBean1->RemoveAllConnections();
      a_pBean2->RemoveAllConnections();

      return;
   }


   if (a_Depth == m_AIThoughtLevel)
   {
      if (m_OptimalMoves.size() == 0)
      {
         m_OptimalMoves.push_back(a_InitialMove);
      }
      else if (m_OptimalMoves.begin()->m_Score == a_InitialMove.m_Score)
      {
         m_OptimalMoves.push_back(a_InitialMove);
      }
      else if (m_OptimalMoves.begin()->m_Score < a_InitialMove.m_Score)
      {
         m_OptimalMoves.clear();
         m_OptimalMoves.push_back(a_InitialMove);
      }
   }
   else
   {
      // Recursively call this function again
      _AnalyzeAllMoves(a_rPlayingField, ++a_Depth, a_InitialMove);
   }

   // SimulatePlay dirtys up the Beans. Restore the bean's position and
   // connections for the next simulation.
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
   uint32_t l_Chains = 0;
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
         l_ReturnScore +=
            _ConnectColumnNeighbors(l_Column, a_rPlayingField);
      }

      std::unordered_set<uint32_t> l_NewColumnsOfInterest;
      for (uint32_t l_Column : l_ColumnsOfInterest)
      {
         uint32_t l_BeansExploded = 0;
         uint32_t l_Multiplier = 0;
         uint32_t l_DifferentGroups = 0;

         _SearchColumnAndExplodeConnections(
            l_Column,
            a_rPlayingField,
            &l_NewColumnsOfInterest,
            &l_BeansExploded,
            &l_Multiplier,
            &l_DifferentGroups
            );

         l_ReturnScore +=
            (2 * CalculateScore(l_BeansExploded, l_Multiplier, l_DifferentGroups, l_Chains));
      }

      l_ColumnsOfInterest = l_NewColumnsOfInterest;
      ++l_Chains;

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

void cAiPlayer::_SearchColumnAndExplodeConnections(
   uint32_t a_Column,
   std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField,
   std::unordered_set<uint32_t>* a_pNewColumnsOfInterest,
   uint32_t* a_BeansExploded,
   uint32_t* a_Multiplier,
   uint32_t* a_DifferentGroups
   )
{
   *a_DifferentGroups = 0;
   *a_Multiplier = 1;
   *a_BeansExploded = 0;

   for (uint32_t l_Row = 0; l_Row < a_rPlayingField[a_Column].size(); ++l_Row)
   {
      if (a_rPlayingField[a_Column][l_Row] != NULL)
      {
         std::unordered_set<cBeanInfo*> l_Connections =
            a_rPlayingField[a_Column][l_Row]->CountConnections();


         if (l_Connections.size() > 3)
         {
            ++(*a_DifferentGroups);
            *a_BeansExploded = l_Connections.size();
            uint32_t l_BeansExplodedThisGroup = l_Connections.size();
            uint32_t l_MultiplierThisGroup = l_BeansExplodedThisGroup - 3;
            if (l_MultiplierThisGroup > *a_Multiplier)
            {
               *a_Multiplier = l_MultiplierThisGroup;
            }

            // Explode this bean and all of the connected beans
            for (cBeanInfo* l_pConnection : l_Connections)
            {
               uint32_t l_DeleteX = l_pConnection->GetGridPosition().x;
               uint32_t l_DeleteY = l_pConnection->GetGridPosition().y;

               l_pConnection->RemoveAllConnections();
               a_rPlayingField[l_DeleteX][l_DeleteY] = NULL;
               a_pNewColumnsOfInterest->insert(l_DeleteX);

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
}

