
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
      SimulatePlay(a_pBean1, a_pBean2, a_rPlayingField);

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
