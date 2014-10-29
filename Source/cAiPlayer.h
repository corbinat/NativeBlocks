#ifndef ___cAiPlayer_h___
#define ___cAiPlayer_h___

#include "cPlayer.h"

#include <thread>

struct sOptimalPosition
{
   sOptimalPosition()
      : m_Score(0),
        m_Column(0),
        m_Rotation(kRotationStateUp)
   {
   }

   uint32_t m_Score;
   uint32_t m_Column;
   eRotationState m_Rotation;
};

class cAiPlayer : public cPlayer
{
public:
   cAiPlayer(cResources* a_pResources, std::minstd_rand a_RandomNumberEngine, std::string a_Identifier);
   ~cAiPlayer();

   void StateChange(ePlayerState a_Old, ePlayerState a_New);
   void ControlBeans(uint32_t a_ElapsedMiliSec);
private:

   void _AnalyzeAllMoves();

   void _AnalyzeAllSecondayMoves(
      std::vector<std::vector<cBeanInfo>> & a_PlayingField,
      uint32_t a_Depth,
      sOptimalPosition a_InitialMove
      );

   // This helper will simulate the given play and analyze the resulting score.
   // If the score is higher than what is in the Optimal Moves vector, then the
   // vector will be cleared and the new play will be added.
   void _AnalyzeMove(
      cBeanInfo & a_pBean1,
      cBeanInfo & a_pBean2,
      eRotationState a_RotationState,
      std::vector<std::vector<cBeanInfo>> a_rPlayingField,
      sOptimalPosition a_InitialMove,
      uint32_t a_Depth
      );

   // This function returns the resulting score if a player were to play the
   // provided beans
   uint32_t _SimulatePlay(
      cBeanInfo a_pBean1,
      cBeanInfo a_pBean2,
      std::vector<std::vector<cBeanInfo>>& a_rPlayingField
      );

   uint32_t _ConnectBeanToNeighbors(
      cBeanInfo & a_pBean,
      std::vector<std::vector<cBeanInfo>>& a_rPlayingField
      );

   uint32_t _ConnectColumnNeighbors(
      uint32_t a_Column,
      std::vector<std::vector<cBeanInfo>>& a_rPlayingField
      );

   bool _BubbleBeansDown(
      std::vector<cBeanInfo>& a_rColumn,
      std::vector<std::vector<cBeanInfo>>& a_rPlayingField
      );

   // returns how many exploded
   void _SearchColumnAndExplodeConnections(
      uint32_t a_Column,
      std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
      std::unordered_set<uint32_t>* a_pNewColumnsOfInterest,
      uint32_t* a_BeansExploded,
      uint32_t* a_Multiplier,
      uint32_t* a_DifferentGroups
      );

   // This function returns true if beans are getting close to landing.
   bool _IsColumnUrgencyHigh(
      std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
      sf::Vector2<uint32_t> a_FallingBean1,
      sf::Vector2<uint32_t> a_FallingBean2
      );

   // This function returns true if beans are getting close to landing at depth 0.
   bool _IsCurrentColumnUrgencyHigh();

   // This function modifies m_DelayToFirstMove and m_AIThoughtLevel depending
   // on how close the AI is to losing. More pressure makes both values go down.
   void _CalculatePressure(
      std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
      sf::Vector2<uint32_t> a_FallingBean1,
      sf::Vector2<uint32_t> a_FallingBean2
      );

   bool m_DoneThinking;
   bool m_StartThinking;
   std::vector<eRotationState> m_RotationStatesToTest;

   // Vector keeps a list of optimal moves
   std::vector<sOptimalPosition> m_OptimalMoves;

   uint32_t m_DelayToFirstMove;
   bool m_FirstMoveMade;
   uint32_t m_DelayToAdditionalMoves;
   uint32_t m_DelayTimer;

   uint32_t m_AIThoughtLevel;
   uint32_t m_MaxAIThoughtLevel;

   // This has to be a pointer because threads can't be reused. I have to
   // reconstruct them after join.
   std::shared_ptr<std::thread> m_AIThinkingThread;

   // This allows the AI to use several steps to think. Ignored if multithreaded
   uint32_t m_AIPass;

   bool m_Multithreaded;
};

#endif
