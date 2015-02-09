#ifndef ___cAiPersonality_h___
#define ___cAiPersonality_h___

#include "cBeanInfo.h"

// sf::Vector2
#include "SFML/System.hpp"

#include <vector>
#include <stdint.h>

enum eAiPersonality
{
   kAiPersonalityEasy,
   kAiPersonalityMedium,
   kAiPersonalityHard,
};

// This function returns true if beans are getting close to landing.
bool IsColumnUrgencyHigh(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
   sf::Vector2<uint32_t> a_FallingBean1,
   sf::Vector2<uint32_t> a_FallingBean2
   );

class cAiPersonality
{
public:
   cAiPersonality(eAiPersonality a_Personality);
   ~cAiPersonality();

   // Gets called when the AI starts thinking to allow parameters to change
   // based on pressure.
   void AdjustPersonalityToState(
      std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
      sf::Vector2<uint32_t> a_FallingBean1,
      sf::Vector2<uint32_t> a_FallingBean2,
      uint32_t a_MiliSecPerFall
      );

   // Lets the AI know that garbage fell in case it wants to adjust its strategy
   void GarbageAdded();

   uint32_t GetDelayToAdditionalMoves();
   uint32_t GetDelayToFirstMove();
   uint32_t GetAIThoughtLevel();
   uint32_t GetHighestScore();

private:

   void _EasyPersonalityAdjustment(
      std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
      sf::Vector2<uint32_t> a_FallingBean1,
      sf::Vector2<uint32_t> a_FallingBean2
      );

   void _MediumPersonalityAdjustment(
      std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
      sf::Vector2<uint32_t> a_FallingBean1,
      sf::Vector2<uint32_t> a_FallingBean2
      );

   void _HardPersonalityAdjustment(
      std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
      sf::Vector2<uint32_t> a_FallingBean1,
      sf::Vector2<uint32_t> a_FallingBean2
      );

   // How many moves ahead the AI can think
   uint32_t m_MaxAIThoughtLevel;
   uint32_t m_CurrentAIThoughtLevel;

   // True if the AI's thought level will count down or not. The point of this
   // is so that the AI can execute its ideas instead of always trying to set up
   // something until it loses. The thought level should reset if garbage falls.
   bool m_EnableCountDownThoughtLevels;

   uint32_t m_MinDelayToFirstMove;
   uint32_t m_MaxDelayToFirstMove;
   uint32_t m_CurrentMaxDelayToFirstMove;
   // Gets randomized during AdjustPersonalityToState
   uint32_t m_CurrentDelayToFirstMove;


   uint32_t m_MinDelayToAdditionalMoves;
   uint32_t m_MaxDelayToAdditionalMoves;
   uint32_t m_CurrentMaxDelayToAdditionalMoves;
   // Gets randomized during AdjustPersonalityToState
   uint32_t m_CurrentDelayToAdditionalMoves;

   // The AI will stop searching for moves once this value is reached. Set to 0
   // if it should not apply.
   uint32_t m_HighestScore;

   eAiPersonality m_Personality;

   uint32_t m_MiliSecPerFall;
};

#endif
