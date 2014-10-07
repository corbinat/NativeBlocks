#ifndef ___cAiPlayer_h___
#define ___cAiPlayer_h___

#include "cPlayer.h"

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

   std::vector<std::vector<std::shared_ptr<cBeanInfo>>> DeepCopyGivenPlayingField(
      std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField
      );

   void _AnalyzeAllMoves(
      std::vector<std::vector<std::shared_ptr<cBeanInfo>>> & a_PlayingField,
      uint32_t a_Depth,
      sOptimalPosition a_InitialMove
   );

   // This helper will simulate the given play and analyze the resulting score.
   // If the score is higher than what is in the Optimal Moves vector, then the
   // vector will be cleared and the new play will be added.
   void _AnalyzeMove(
      std::shared_ptr<cBeanInfo> a_pBean1,
      std::shared_ptr<cBeanInfo> a_pBean2,
      eRotationState a_RotationState,
      std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField,
      sOptimalPosition a_InitialMove,
      uint32_t a_Depth
      );

   // This function returns the resulting score if a player were to play the
   // provided beans
   uint32_t _SimulatePlay(
      std::shared_ptr<cBeanInfo> a_pBean1,
      std::shared_ptr<cBeanInfo> a_pBean2,
      std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField
      );

   uint32_t _ConnectBeanToNeighbors(
      std::shared_ptr<cBeanInfo> a_pBean,
      std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField
      );

   uint32_t _ConnectColumnNeighbors(
      uint32_t a_Column,
      std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField
      );

   bool _BubbleBeansDown(std::vector<std::shared_ptr<cBeanInfo>>& a_Column);

   // returns how many exploded
   void _SearchColumnAndExplodeConnections(
      uint32_t a_Column,
      std::vector<std::vector<std::shared_ptr<cBeanInfo>>>& a_rPlayingField,
      std::unordered_set<uint32_t>* a_pNewColumnsOfInterest,
      uint32_t* a_BeansExploded,
      uint32_t* a_Multiplier,
      uint32_t* a_DifferentGroups
      );


   bool m_DoneThinking;

   // Vector keeps a list of optimal moves
   std::vector<sOptimalPosition> m_OptimalMoves;

   uint32_t m_DelayToFirstMove;
   uint32_t m_DelayTimer;

   uint32_t m_AIThoughtLevel;
};

#endif
