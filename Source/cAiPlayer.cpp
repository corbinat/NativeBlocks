
#include "cAiPlayer.h"
#include "cBean.h"
#include "cBeanInfo.h"

#include <iostream>

cAiPlayer::cAiPlayer(cResources* a_pResources, std::minstd_rand a_RandomNumberEngine, std::string a_Identifier)
   : cPlayer(a_pResources, a_RandomNumberEngine, a_Identifier),
     m_OptimalMoves(),
     m_DoneThinking(false),
     m_StartThinking(false),
     m_DelayToFirstMove(0),
     m_FirstMoveMade(false),
     m_DelayToAdditionalMoves(0),
     m_DelayTimer(0),
     m_AIThoughtLevel(1),
     m_MaxAIThoughtLevel(1),
     m_AIPass(0),
     m_Multithreaded(true),
     m_AIThinkingThread(NULL)
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
      m_StartThinking = true;

      if (!m_Multithreaded)
      {
         _AnalyzeAllMoves();
      }
      else
      {
         m_AIThinkingThread.reset(
            new std::thread (&cAiPlayer::_AnalyzeAllMoves, this)
         );
      }
   }
   else
   {
      m_StartThinking = false;
      m_OptimalMoves.clear();
      m_DoneThinking = false;
      SetFastFall(false);
   }
}

void cAiPlayer::ControlBeans(uint32_t a_ElapsedMiliSec)
{
   // Move to where we settle
   if (!m_StartThinking)
   {
      return;
   }

   if (!m_DoneThinking && !m_Multithreaded)
   {
      _AnalyzeAllMoves();
      return;
   }

   if (!m_DoneThinking && m_Multithreaded)
   {
      // wait for the AI to finish thinking if urgency is getting high
      if (_IsCurrentColumnUrgencyHigh())
      {
         std::cout << "JOIN 1" << std::endl;
         if (!m_AIThinkingThread->joinable())
         {
            std::cout << "WHY NOT JOINABLE??????? CRASHHHH" << std::endl;
         }
         m_AIThinkingThread->join();
         std::cout << "JOIN 1 Done" << std::endl;
      }
      else
      {
         return;
      }
   }
   else if (m_DoneThinking && m_Multithreaded && m_AIThinkingThread->joinable())
   {
      //std::cout << "JOIN 2" << std::endl;
      m_AIThinkingThread->join();
      //std::cout << "JOIN 2 Done" << std::endl;
   }


   // If I get here then the AI is done thinking.
   m_DelayTimer += a_ElapsedMiliSec;
   if (m_FirstMoveMade)
   {
      if (m_DelayTimer < m_DelayToAdditionalMoves)
      {
         return;
      }
   }
   else
   {
      if (m_DelayTimer < m_DelayToFirstMove)
      {
         return;
      }
   }
   m_FirstMoveMade = true;

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

   if (!l_Success && !_IsCurrentColumnUrgencyHigh())
   {
      // AI couldn't move where it wanted due to a wall of beans. Start the
      // search over. _IsCurrentColumnUrgencyHigh returned false so we know we
      // have time to search again.
      m_OptimalMoves.clear();
      std::cout << "AHHHHH START OVER" << l_Destination.m_Column << " " << l_Destination.m_Rotation << std::endl;

       if (!m_Multithreaded)
      {
         _AnalyzeAllMoves();
      }
      else
      {
         if (m_AIThinkingThread->joinable())
         {
            std::cout << "WHAT THE HECKKKKKK" << std::endl;
         }
         m_AIThinkingThread.reset(
            new std::thread (&cAiPlayer::_AnalyzeAllMoves, this)
         );
      }
   }

}

void cAiPlayer::_AnalyzeAllMoves()
{

   std::vector<std::vector<cBeanInfo>> l_PlayingField =
      ClonePlayingField();

   m_DoneThinking = false;

   cBeanInfo l_PivotBean;
   sf::Vector2<uint32_t> l_PivotPosition;
   cBeanInfo l_SwingBean;
   sf::Vector2<uint32_t> l_SwingPosition;

    // Copy the current beans in play
   sf::Vector2<cBean*> l_BeansInPlay = GetBeansInPlay();

   l_PivotBean.SetColor(l_BeansInPlay.x->GetColor());
   l_SwingBean.SetColor(l_BeansInPlay.y->GetColor());

   l_PivotPosition =
      GetBeanGridPosition(l_BeansInPlay.x);

   // Start the beans in the up position
   l_SwingPosition.x = l_PivotPosition.x;
   l_SwingPosition.y = l_PivotPosition.y - 1;

   // Determine what kind of pressure the AI is under
   _CalculatePressure(l_PlayingField, l_PivotPosition, l_SwingPosition);

   std::vector <eRotationState> l_RotationsToTest;
   l_RotationsToTest.push_back(kRotationStateUp);
   l_RotationsToTest.push_back(kRotationStateDown);
   l_RotationsToTest.push_back(kRotationStateLeft);
   l_RotationsToTest.push_back(kRotationStateRight);

   for (eRotationState l_RotationState : l_RotationsToTest)
   {
      l_PivotBean.SetGridPosition(l_PivotPosition);
      l_SwingBean.SetGridPosition(l_SwingPosition);

      // Adjust the swing bean for each rotation state
      uint32_t l_MinRow = 0;
      uint32_t l_MaxRow = 5;

      if (l_RotationState == kRotationStateLeft)
      {
         if (l_SwingBean.GetGridPosition().x > l_MinRow && l_PlayingField[l_PivotBean.GetGridPosition().x - 1][l_PivotBean.GetGridPosition().y].GetColor() == kBeanColorEmpty)
         {
            l_SwingBean.SetColumnPosition(l_PivotBean.GetGridPosition().x - 1);
            l_SwingBean.SetRowPosition(l_PivotBean.GetGridPosition().y);
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
         if (l_SwingBean.GetGridPosition().x < l_MaxRow && l_PlayingField[l_PivotBean.GetGridPosition().x + 1][l_PivotBean.GetGridPosition().y].GetColor() == kBeanColorEmpty)
         {
            l_SwingBean.SetColumnPosition(l_PivotBean.GetGridPosition().x + 1);
            l_SwingBean.SetRowPosition(l_PivotBean.GetGridPosition().y);
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
         if (l_PivotBean.GetColor() == l_SwingBean.GetColor())
         {
            // This is same state as up if both beans have same color.
            continue;
         }
         if (l_PlayingField[l_PivotBean.GetGridPosition().x][l_PivotBean.GetGridPosition().y + 1].GetColor() == kBeanColorEmpty)
         {
            l_SwingBean.SetRowPosition(l_PivotBean.GetGridPosition().y + 1);
         }
         else
         {
            // we can't rotate down easily.
            continue;
         }
      }

      uint32_t l_Depth = 0;
      sOptimalPosition l_InitialMove;

      _AnalyzeMove(
         l_PivotBean,
         l_SwingBean,
         l_RotationState,
         l_PlayingField,
         l_InitialMove,
         l_Depth
         );

      // Simulate to the left.
      while (l_PivotBean.GetGridPosition().x > l_MinRow)
      {
         // See if we can even go left
         if (  l_PlayingField[l_PivotBean.GetGridPosition().x - 1][l_PivotBean.GetGridPosition().y].GetColor() != kBeanColorEmpty
            || l_PlayingField[l_SwingBean.GetGridPosition().x - 1][l_SwingBean.GetGridPosition().y].GetColor() != kBeanColorEmpty
            )
         {
            break;
         }

         l_PivotBean.SetColumnPosition(l_PivotBean.GetGridPosition().x - 1);
         l_SwingBean.SetColumnPosition(l_SwingBean.GetGridPosition().x - 1);

         _AnalyzeMove(
            l_PivotBean,
            l_SwingBean,
            l_RotationState,
            l_PlayingField,
            l_InitialMove,
            l_Depth
            );
      }

      // Reset to middle to prepare for going right
      l_PivotBean.SetGridPosition(l_PivotPosition);
      l_SwingBean.SetGridPosition(l_SwingPosition);
      if (l_RotationState == kRotationStateLeft)
      {
         l_SwingBean.SetColumnPosition(l_PivotBean.GetGridPosition().x - 1);
         l_SwingBean.SetRowPosition(l_PivotBean.GetGridPosition().y);

         l_MinRow = 1;
      }
      else if (l_RotationState == kRotationStateRight)
      {
         l_SwingBean.SetColumnPosition(l_PivotBean.GetGridPosition().x + 1);
         l_SwingBean.SetRowPosition(l_PivotBean.GetGridPosition().y);

         l_MaxRow = 4;
      }
      else if (l_RotationState == kRotationStateDown)
      {
         l_SwingBean.SetColumnPosition(l_PivotBean.GetGridPosition().x);
         l_SwingBean.SetRowPosition(l_PivotBean.GetGridPosition().y + 1);
      }

      // Simulate to the right
      while (l_PivotBean.GetGridPosition().x < l_MaxRow)
      {
         // See if we can even go right
         if (  l_PlayingField[l_PivotBean.GetGridPosition().x + 1][l_PivotBean.GetGridPosition().y].GetColor() != kBeanColorEmpty
            || l_PlayingField[l_SwingBean.GetGridPosition().x + 1][l_SwingBean.GetGridPosition().y].GetColor() != kBeanColorEmpty
            )
         {
            break;
         }

         l_PivotBean.SetColumnPosition(l_PivotBean.GetGridPosition().x + 1);
         l_SwingBean.SetColumnPosition(l_SwingBean.GetGridPosition().x + 1);

         _AnalyzeMove(
            l_PivotBean,
            l_SwingBean,
            l_RotationState,
            l_PlayingField,
            l_InitialMove,
            l_Depth
            );
      }

   }

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
         //~ std::cout << "Upcoming Beans: ";
         //~ for (int i = 0; i < 3 ; ++ i)
         //~ {
            //~ std::cout << m_Staging.InspectNextBeanColor(i * 2);
            //~ std::cout << m_Staging.InspectNextBeanColor(i * 2 + 1);
         //~ }

         std::cout << std::endl;
         std::cout << l_Final.m_Column << " " << l_Final.m_Rotation << std::endl;
      }
   }

   m_DoneThinking = true;
   std::cout << "Done thinking" << std::endl;
   m_FirstMoveMade = false;

   //~ std::uniform_int_distribution<int> l_Distribution2(200, 2000);
   //~ m_DelayToFirstMove = l_Distribution2(l_Generator);
   //~ m_DelayToFirstMove = l_Distribution2(l_Generator);
   m_DelayTimer = 0;
}


void cAiPlayer::_AnalyzeAllSecondayMoves(
   std::vector<std::vector<cBeanInfo>> & a_PlayingField,
   uint32_t a_Depth,
   sOptimalPosition a_InitialMove
   )
{
   if (a_Depth > m_AIThoughtLevel)
   {
      return;
   }

   cBeanInfo l_PivotBean;
   sf::Vector2<uint32_t> l_PivotPosition;
   cBeanInfo l_SwingBean;
   sf::Vector2<uint32_t> l_SwingPosition;

   eBeanColor l_Color = m_Staging.InspectNextBeanColor(a_Depth * 2 - 2);
   eBeanColor l_Color2 = m_Staging.InspectNextBeanColor(a_Depth * 2 + 1 - 2);
   //std::cout << "AI color: " << l_Color << "," << l_Color2 << std::endl;

   l_PivotBean.SetColor(l_Color);
   l_SwingBean.SetColor(l_Color2);

   l_PivotPosition.x = 2;
   l_PivotPosition.y = 4;

   l_SwingPosition.x = 2;
   l_SwingPosition.y = 3;

   l_PivotBean.SetGridPosition(l_PivotPosition);
   l_SwingBean.SetGridPosition(l_SwingPosition);

   // Adjust the swing bean for each rotation state
   uint32_t l_MinRow = 0;
   uint32_t l_MaxRow = 5;

   bool l_SkipThisRotation = false;

   std::vector<eRotationState>l_RotationStates;
   l_RotationStates.push_back(kRotationStateDown);
   l_RotationStates.push_back(kRotationStateUp);
   l_RotationStates.push_back(kRotationStateRight);
   l_RotationStates.push_back(kRotationStateLeft);

   for (eRotationState l_RotationState : l_RotationStates)
   {

      if (l_RotationState == kRotationStateLeft)
      {
         if (l_SwingBean.GetGridPosition().x > l_MinRow && a_PlayingField[l_PivotBean.GetGridPosition().x - 1][l_PivotBean.GetGridPosition().y].GetColor() == kBeanColorEmpty)
         {
            l_SwingBean.SetColumnPosition(l_PivotBean.GetGridPosition().x - 1);
            l_SwingBean.SetRowPosition(l_PivotBean.GetGridPosition().y);
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
         if (l_SwingBean.GetGridPosition().x < l_MaxRow && a_PlayingField[l_PivotBean.GetGridPosition().x + 1][l_PivotBean.GetGridPosition().y].GetColor() == kBeanColorEmpty)
         {
            l_SwingBean.SetColumnPosition(l_PivotBean.GetGridPosition().x + 1);
            l_SwingBean.SetRowPosition(l_PivotBean.GetGridPosition().y);
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
         if (l_PivotBean.GetColor() == l_SwingBean.GetColor())
         {
            // This is same state as up if both beans have same color.
            continue;
         }
         if (a_PlayingField[l_PivotBean.GetGridPosition().x][l_PivotBean.GetGridPosition().y + 1].GetColor() == kBeanColorEmpty)
         {
            l_SwingBean.SetRowPosition(l_PivotBean.GetGridPosition().y + 1);
         }
         else
         {
            // we can't rotate down easily.
            continue;
         }
      }

      _AnalyzeMove(
         l_PivotBean,
         l_SwingBean,
         l_RotationState,
         a_PlayingField,
         a_InitialMove,
         a_Depth
         );

      // Simulate to the left.
      while (l_PivotBean.GetGridPosition().x > l_MinRow)
      {
         // See if we can even go left
         if (  a_PlayingField[l_PivotBean.GetGridPosition().x - 1][l_PivotBean.GetGridPosition().y].GetColor() != kBeanColorEmpty
            || a_PlayingField[l_SwingBean.GetGridPosition().x - 1][l_SwingBean.GetGridPosition().y].GetColor() != kBeanColorEmpty
            )
         {
            break;
         }

         l_PivotBean.SetColumnPosition(l_PivotBean.GetGridPosition().x - 1);
         l_SwingBean.SetColumnPosition(l_SwingBean.GetGridPosition().x - 1);

         _AnalyzeMove(
            l_PivotBean,
            l_SwingBean,
            l_RotationState,
            a_PlayingField,
            a_InitialMove,
            a_Depth
            );
      }

      // Reset to middle to prepare for going right
      l_PivotBean.SetGridPosition(l_PivotPosition);
      l_SwingBean.SetGridPosition(l_SwingPosition);
      if (l_RotationState == kRotationStateLeft)
      {
         l_SwingBean.SetColumnPosition(l_PivotBean.GetGridPosition().x - 1);
         l_SwingBean.SetRowPosition(l_PivotBean.GetGridPosition().y);

         l_MinRow = 1;
      }
      else if (l_RotationState == kRotationStateRight)
      {
         l_SwingBean.SetColumnPosition(l_PivotBean.GetGridPosition().x + 1);
         l_SwingBean.SetRowPosition(l_PivotBean.GetGridPosition().y);

         l_MaxRow = 4;
      }
      else if (l_RotationState == kRotationStateDown)
      {
         l_SwingBean.SetColumnPosition(l_PivotBean.GetGridPosition().x);
         l_SwingBean.SetRowPosition(l_PivotBean.GetGridPosition().y + 1);
      }

      // Simulate to the right
      while (l_PivotBean.GetGridPosition().x < l_MaxRow)
      {
         // See if we can even go right
         if (  a_PlayingField[l_PivotBean.GetGridPosition().x + 1][l_PivotBean.GetGridPosition().y].GetColor() != kBeanColorEmpty
            || a_PlayingField[l_SwingBean.GetGridPosition().x + 1][l_SwingBean.GetGridPosition().y].GetColor() != kBeanColorEmpty
            )
         {
            break;
         }

         l_PivotBean.SetColumnPosition(l_PivotBean.GetGridPosition().x + 1);
         l_SwingBean.SetColumnPosition(l_SwingBean.GetGridPosition().x + 1);

         _AnalyzeMove(
            l_PivotBean,
            l_SwingBean,
            l_RotationState,
            a_PlayingField,
            a_InitialMove,
            a_Depth
            );
      }
   }
}



void cAiPlayer::_AnalyzeMove(
   cBeanInfo & a_rBean1,
   cBeanInfo & a_rBean2,
   eRotationState a_RotationState,
   std::vector<std::vector<cBeanInfo>> a_PlayingField,
   sOptimalPosition a_InitialMove,
   uint32_t a_Depth
   )
{
   sf::Vector2<uint32_t> l_PivotPosition = a_rBean1.GetGridPosition();
   sf::Vector2<uint32_t> l_SwingPosition = a_rBean2.GetGridPosition();

   if (a_Depth == 0)
   {
      a_InitialMove.m_Column = a_rBean1.GetGridPosition().x;
      a_InitialMove.m_Rotation = a_RotationState;
   }

   a_InitialMove.m_Score =
      _SimulatePlay(a_rBean1, a_rBean2, a_PlayingField) + a_InitialMove.m_Score;

   //~ if (a_InitialMove.m_Score != 0 && a_Depth == 0)
   //~ {
      //~ // Add a bonus to doing things ealier
      //~ a_InitialMove.m_Score *= 2;
   //~ }

   // See if we have lost. If so, don't bother continuing on.
   if (a_PlayingField[2][5].GetColor() != kBeanColorEmpty)
   {
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
      _AnalyzeAllSecondayMoves(a_PlayingField, ++a_Depth, a_InitialMove);
   }
}

uint32_t cAiPlayer::_SimulatePlay(
   cBeanInfo a_Bean1,
   cBeanInfo a_Bean2,
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField
   )
{
   // Keep track of columns that get modified so that we can check them for
   // matches and things
   std::unordered_set<uint32_t> l_ColumnsOfInterest;

   l_ColumnsOfInterest.insert(a_Bean1.GetGridPosition().x);
   l_ColumnsOfInterest.insert(a_Bean2.GetGridPosition().x);

   a_rPlayingField[a_Bean1.GetGridPosition().x][a_Bean1.GetGridPosition().y] = a_Bean1;
   a_rPlayingField[a_Bean2.GetGridPosition().x][a_Bean2.GetGridPosition().y] = a_Bean2;

   // Search through the columns and explode any beans with 4 connected
   uint32_t l_ReturnScore = 0;

   // _Bubbledown new columns of interest, connect neighbors, and explode again
   uint32_t l_Chains = 0;
   while (l_ColumnsOfInterest.size() != 0)
   {
      for (uint32_t l_Column : l_ColumnsOfInterest)
      {
         _BubbleBeansDown(a_rPlayingField[l_Column], a_rPlayingField);

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
   cBeanInfo & a_pBean,
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField
   )
{
   uint32_t l_ReturnConnections = 0;

   if (a_pBean.GetColor() == kBeanColorGarbage)
   {
      return l_ReturnConnections;
   }

   uint32_t l_X = a_pBean.GetGridPosition().x;
   uint32_t l_Y = a_pBean.GetGridPosition().y;
   if (l_X > 0)
   {
      cBeanInfo & l_pNeighbor = a_rPlayingField[l_X - 1][l_Y];
      if (l_pNeighbor.GetColor() == a_pBean.GetColor())
      {
         if (a_pBean.AddConnection(&l_pNeighbor))
         {
            ++l_ReturnConnections;
         }
      }
   }
   if (l_X < 5)
   {
      cBeanInfo & l_pNeighbor = a_rPlayingField[l_X + 1][l_Y];
      if (l_pNeighbor.GetColor() == a_pBean.GetColor())
      {
         if (a_pBean.AddConnection(&l_pNeighbor))
         {
            ++l_ReturnConnections;
         }
      }
   }
   if (l_Y > 0)
   {
      cBeanInfo & l_pNeighbor = a_rPlayingField[l_X][l_Y - 1];
      if (l_pNeighbor.GetColor() == a_pBean.GetColor())
      {
         if (a_pBean.AddConnection(&l_pNeighbor))
         {
            ++l_ReturnConnections;
         }
      }
   }
   if (l_Y < g_kTotalRows - 1)
   {
      cBeanInfo & l_pNeighbor = a_rPlayingField[l_X][l_Y + 1];
      if (l_pNeighbor.GetColor() == a_pBean.GetColor())
      {
         if (a_pBean.AddConnection(&l_pNeighbor))
         {
            ++l_ReturnConnections;
         }
      }
   }

   return l_ReturnConnections;
}

uint32_t cAiPlayer::_ConnectColumnNeighbors(
   uint32_t a_Column,
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField
   )
{
   uint32_t l_ReturnConnections = 0;

   for (uint32_t l_Row = 0; l_Row < a_rPlayingField[a_Column].size(); ++l_Row)
   {
      if (a_rPlayingField[a_Column][l_Row].GetColor() != kBeanColorEmpty)
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

bool cAiPlayer::_BubbleBeansDown(
   std::vector<cBeanInfo>& a_rColumn,
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField
   )
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
      if (a_rColumn[i].GetColor() == kBeanColorEmpty)
      {
         if (l_SawNull == false && i < l_FirstNull)
         {
            l_FirstNull = i;
         }
         l_SawNull = true;
      }
      else if (l_SawNull)
      {
         a_rColumn[i].RemoveAllConnections(a_rPlayingField);
         cBeanInfo l_Element = a_rColumn[i];

         a_rColumn[i] = a_rColumn[i + 1];
         a_rColumn[i + 1] = l_Element;
         a_rColumn[i + 1].SetRowPosition(i + 1);
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
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
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
      if (a_rPlayingField[a_Column][l_Row].GetColor() != kBeanColorEmpty)
      {
         std::unordered_set<cBeanInfo*> l_Connections =
            a_rPlayingField[a_Column][l_Row].CountConnections(a_rPlayingField);

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

               l_pConnection->RemoveAllConnections(a_rPlayingField);
               a_rPlayingField[l_DeleteX][l_DeleteY] = kBeanColorEmpty;
               a_pNewColumnsOfInterest->insert(l_DeleteX);

               // Delete garbage beans that are touching
               if (l_DeleteX > 0)
               {
                  cBeanInfo & l_pNeighbor = a_rPlayingField[l_DeleteX - 1][l_DeleteY];
                  if (l_pNeighbor.GetColor() == kBeanColorGarbage)
                  {
                     a_rPlayingField[l_DeleteX - 1][l_DeleteY].SetColor(kBeanColorEmpty);
                     a_rPlayingField[l_DeleteX - 1][l_DeleteY].RemoveAllConnections(a_rPlayingField);
                     a_pNewColumnsOfInterest->insert(l_DeleteX - 1);
                  }
               }
               if (l_DeleteX < 5)
               {
                  cBeanInfo & l_pNeighbor = a_rPlayingField[l_DeleteX + 1][l_DeleteY];
                  if (l_pNeighbor.GetColor() == kBeanColorGarbage)
                  {
                     a_rPlayingField[l_DeleteX + 1][l_DeleteY].SetColor(kBeanColorEmpty);
                     a_rPlayingField[l_DeleteX + 1][l_DeleteY].RemoveAllConnections(a_rPlayingField);
                     a_pNewColumnsOfInterest->insert(l_DeleteX + 1);
                  }
               }
               if (l_DeleteY > 0)
               {
                  cBeanInfo & l_pNeighbor = a_rPlayingField[l_DeleteX][l_DeleteY - 1];
                  if (l_pNeighbor.GetColor() == kBeanColorGarbage)
                  {
                     a_rPlayingField[l_DeleteX][l_DeleteY - 1].SetColor(kBeanColorEmpty);
                     a_rPlayingField[l_DeleteX][l_DeleteY - 1].RemoveAllConnections(a_rPlayingField);
                  }
               }
               if (l_DeleteY < g_kTotalRows - 1)
               {
                  cBeanInfo & l_pNeighbor = a_rPlayingField[l_DeleteX][l_DeleteY + 1];
                  if (l_pNeighbor.GetColor() == kBeanColorGarbage)
                  {
                     a_rPlayingField[l_DeleteX][l_DeleteY + 1].SetColor(kBeanColorEmpty);
                     a_rPlayingField[l_DeleteX][l_DeleteY + 1].RemoveAllConnections(a_rPlayingField);
                  }
               }
            }
         }
      }
   }
}

bool cAiPlayer::_IsColumnUrgencyHigh(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
   sf::Vector2<uint32_t> a_FallingBean1,
   sf::Vector2<uint32_t> a_FallingBean2
   )
{
   // If the nearest bean in our column is within 2 then the pressure is
   // automatically high.
   uint32_t l_Column1 = a_FallingBean1.x;

   uint32_t i;
   for (i = 0; i < a_rPlayingField[l_Column1].size(); ++i)
   {
      if (a_rPlayingField[l_Column1][i].GetColor() != kBeanColorEmpty)
      {
         break;
      }
   }

   if (i != a_rPlayingField[l_Column1].size())
   {
      if (
         (static_cast<int32_t>(a_rPlayingField[l_Column1][i].GetGridPosition().y)
         - static_cast<int32_t>(a_FallingBean1.y))
         < 2
         )
      {
         return true;
      }
   }

   // If the nearest bean in our column is within 2 then the pressure is
   // automatically high.
   uint32_t l_Column2 = a_FallingBean2.x;

   for (i = 0; i < a_rPlayingField[l_Column2].size(); ++i)
   {
      if (a_rPlayingField[l_Column2][i].GetColor() != kBeanColorEmpty)
      {
         break;
      }
   }

   if (i != a_rPlayingField[l_Column1].size())
   {
      if (
         (static_cast<int32_t>(a_rPlayingField[l_Column2][i].GetGridPosition().y)
         - static_cast<int32_t>(a_FallingBean1.y))
         < 2
         )
      {
         return true;
      }
   }

   return false;
}

bool cAiPlayer::_IsCurrentColumnUrgencyHigh()
{
   std::vector<std::vector<cBeanInfo>> l_PlayingField =
      ClonePlayingField();

   sf::Vector2<uint32_t> l_PivotPosition;
   sf::Vector2<uint32_t> l_SwingPosition;

    // Copy the current beans in play
   sf::Vector2<cBean*> l_BeansInPlay = GetBeansInPlay();

   l_PivotPosition =
      GetBeanGridPosition(l_BeansInPlay.x);
   l_SwingPosition =
      GetBeanGridPosition(l_BeansInPlay.y);

   return _IsColumnUrgencyHigh(l_PlayingField, l_PivotPosition, l_SwingPosition);
}

void cAiPlayer::_CalculatePressure(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
   sf::Vector2<uint32_t> a_FallingBean1,
   sf::Vector2<uint32_t> a_FallingBean2
   )
{
   if (_IsColumnUrgencyHigh(a_rPlayingField, a_FallingBean1, a_FallingBean2))
   {
      std::cout << "Very high pressure" << std::endl;
      m_DelayToFirstMove = 0;
      m_DelayToAdditionalMoves = 0;
      m_AIThoughtLevel = 0;
      return;
   }

   // No beans right under us, so base the pressure off of average height of the
   // columns. This average is actually the average number of empty spaces per
   // column.
   uint32_t l_Average = 0;
   for (int32_t l_Column = 0; l_Column < a_rPlayingField.size(); ++l_Column)
   {
      int32_t l_Row;
      for (l_Row = a_rPlayingField[l_Column].size() - 1; l_Row >= 0; --l_Row)
      {
         if (a_rPlayingField[l_Column][l_Row].GetColor() == kBeanColorEmpty)
         {
            break;
         }
      }

      l_Average += l_Row;
   }

   l_Average /= 6;

   // If bean level is high then the pressure is high. Add 5 to account for
   // garbage rows
   if (l_Average < (3 + 5))
   {
      // wait up to 1 bean falls before making the first move
      std::cout << "High Pressure" << std::endl;
      m_DelayToFirstMove = m_MiliSecPerFall / 4;
      m_DelayToAdditionalMoves = m_MiliSecPerFall / 6;
      m_AIThoughtLevel = 0;
   }
   // If bean level is midway then the pressure is up a just a bit. Add 5 to
   // account for garbage rows
   else if (l_Average < (4 + 5))
   {
      std::cout << "Med Pressure" << std::endl;
      // wait up to 1 bean falls before making the first move
      m_DelayToFirstMove = m_MiliSecPerFall;
      m_DelayToAdditionalMoves = m_MiliSecPerFall / 4;
      if (m_MaxAIThoughtLevel > 0)
      {
         m_AIThoughtLevel = m_MaxAIThoughtLevel - 1;
      }
   }
   else
   {
      // wait up to 2 bean falls before making the first move
      m_DelayToFirstMove = m_MiliSecPerFall * 2;
      m_DelayToAdditionalMoves = m_MiliSecPerFall / 4;
      m_AIThoughtLevel = m_MaxAIThoughtLevel;
   }
}
