#ifndef ___cAiPersonality_h___
#define ___cAiPersonality_h___

#include "cBeanInfo.h"

// sf::Vector2
#include "SFML/System.hpp"

#include <vector>
#include <stdint.h>

enum eAiPersonality
{
   kAiPersonalityBeginner,
   kAiPersonalityBeginner2,
   kAiPersonalityBeginner3,
   kAiPersonalityEasy,
   kAiPersonalityEasy2,
   kAiPersonalityEasy3,
   kAiPersonalityMedium,
   kAiPersonalityMedium2,
   kAiPersonalityMedium3,
   kAiPersonalityHard,
};

// This function returns the number of spaces between the falling beans and the
// resting beans.
uint32_t ColumnFreeSpace(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
   sf::Vector2<uint32_t> a_FallingBean1,
   sf::Vector2<uint32_t> a_FallingBean2
   );

uint32_t GetEmpySpaceAverage(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField
   );

class cAiPersonality
{
public:
   cAiPersonality(std::string a_Personality);
   ~cAiPersonality();

   static std::vector<std::string> GetAINames();

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
   uint32_t GetDelayToFastFall();
   uint32_t GetAIThoughtLevel();
   int32_t GetHighestScore();
   uint32_t GetOptimalMoveOdds();
   bool IsRotationLocked();

private:

   void _BeginnerPersonalityAdjustment(
      std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
      sf::Vector2<uint32_t> a_FallingBean1,
      sf::Vector2<uint32_t> a_FallingBean2
      );

   void _Beginner2PersonalityAdjustment(
      std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
      sf::Vector2<uint32_t> a_FallingBean1,
      sf::Vector2<uint32_t> a_FallingBean2
      );

   void _Beginner3PersonalityAdjustment(
      std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
      sf::Vector2<uint32_t> a_FallingBean1,
      sf::Vector2<uint32_t> a_FallingBean2
      );

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

   void _HardestPersonalityAdjustment(
      std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
      sf::Vector2<uint32_t> a_FallingBean1,
      sf::Vector2<uint32_t> a_FallingBean2
      );

   void _AdjustThoughtLevel(uint32_t a_Max, uint32_t a_Min = 0);

   uint32_t _AdjustAITiming(
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
   // Update: this actually makes the AI significantly worse. Medium is about 50%
   // against a countdown hard
   bool m_EnableCountDownThoughtLevels;

   // Turn this on if the AI shouldn't rotate blocks. This is used by one of the
   // fast easy AIs
   bool m_LockRotation;

   uint32_t m_CurrentMinDelayToFirstMove;
   uint32_t m_CurrentMaxDelayToFirstMove;
   // Gets randomized during AdjustPersonalityToState
   uint32_t m_CurrentDelayToFirstMove;


   uint32_t m_CurrentMinDelayToAdditionalMoves;
   uint32_t m_CurrentMaxDelayToAdditionalMoves;
   // Gets randomized during AdjustPersonalityToState
   uint32_t m_CurrentDelayToAdditionalMoves;

   uint32_t m_MinDelayToFastFall;
   uint32_t m_MaxDelayToFastFall;
   uint32_t m_CurrentDelayToFastFall;

   // The AI will stop searching for moves once this value is reached. Set to 0
   // if it should not apply.
   int32_t m_HighestScore;

   // The odds that the AI will only consider the most optimal moves, in
   // percentage. 100 means that AI always uses optimal moves.
   uint32_t m_OptimalMoveOdds;

   eAiPersonality m_Personality;

   uint32_t m_MiliSecPerFall;
};

#endif
