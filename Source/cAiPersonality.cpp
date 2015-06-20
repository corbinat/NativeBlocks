#include "cAiPersonality.h"
#include "cSpeedController.h"

#include <iostream>
#include <chrono>

const uint32_t g_kMinFirstDelayDefault = 30;
const uint32_t g_kMinAdditionalDelayDefault = 30;

const char * const g_kBeginnerAIName = "Amos";

// Carelessly places blocks for a while
const char * const g_kBeginner2AIName = "Bundt";
const char * const g_kBeginner3AIName = "Drub";
const char * const g_kEasyAIName = "EeBoo";
const char * const g_kEasy2AIName = "Nix";
const char * const g_kEasy3AIName = "Rolla";
const char * const g_kMediumAIName = "Broot";
const char * const g_kMedium2AIName = "Triton";
const char * const g_kMedium3AIName = "Vesta";
const char * const g_kHardAIName = "Wolf";

cAiPersonality::cAiPersonality(std::string a_Personality)
   : m_MaxAIThoughtLevel(0),
     m_CurrentAIThoughtLevel(0),
     m_EnableCountDownThoughtLevels(false),
     m_LockRotation(false),
     m_CurrentMinDelayToFirstMove(30),
     m_CurrentMaxDelayToFirstMove(0),
     m_CurrentDelayToFirstMove(0),
     m_CurrentMinDelayToAdditionalMoves(50),
     m_CurrentMaxDelayToAdditionalMoves(0),
     m_CurrentDelayToAdditionalMoves(0),
     m_MinDelayToFastFall(0),
     m_MaxDelayToFastFall(0),
     m_CurrentDelayToFastFall(0),
     m_HighestScore(-1),
     m_OptimalMoveOdds(100),
     m_Personality(kAiPersonalityBeginner),
     m_MiliSecPerFall(100)
{

   if (a_Personality == g_kBeginnerAIName)
   {
      m_Personality = kAiPersonalityBeginner;
      m_MaxAIThoughtLevel = 0;
      m_EnableCountDownThoughtLevels = false;
   }
   else if (a_Personality == g_kBeginner2AIName)
   {
      m_Personality = kAiPersonalityBeginner2;
      m_MaxAIThoughtLevel = 0;
      m_EnableCountDownThoughtLevels = false;
   }
   else if (a_Personality == g_kBeginner3AIName)
   {
      m_Personality = kAiPersonalityBeginner3;
      m_MaxAIThoughtLevel = 0;
      m_EnableCountDownThoughtLevels = false;
   }
   else if (a_Personality == g_kEasyAIName)
   {
      m_Personality = kAiPersonalityEasy;
      m_MaxAIThoughtLevel = 0;
      m_EnableCountDownThoughtLevels = false;
   }
   else if (a_Personality == g_kEasy2AIName)
   {
      m_Personality = kAiPersonalityEasy2;
      m_MaxAIThoughtLevel = 1;
      m_EnableCountDownThoughtLevels = true;

      // For every 140 points we can drop a garbage block. Limit to 6
      m_HighestScore = 6 * 140;
   }
   else if (a_Personality == g_kEasy3AIName)
   {
      m_Personality = kAiPersonalityEasy3;
      m_MaxAIThoughtLevel = 1;
      m_EnableCountDownThoughtLevels = true;

      // Don't limit high score
      m_HighestScore = -1;
   }
   else if (a_Personality == g_kMediumAIName)
   {
      m_Personality = kAiPersonalityMedium;
      m_MaxAIThoughtLevel = 1;
      m_EnableCountDownThoughtLevels = false;

      // For every 140 points we can drop a garbage block. Limit to 8
      m_HighestScore = 8 * 140;
   }
   else if (a_Personality == g_kMedium2AIName)
   {
      m_Personality = kAiPersonalityMedium2;
      m_MaxAIThoughtLevel = 1;
      m_EnableCountDownThoughtLevels = false;

      // Don't limit high score
      m_HighestScore = -1;
   }
   else if (a_Personality == g_kMedium3AIName)
   {
      m_Personality = kAiPersonalityMedium3;
      m_MaxAIThoughtLevel = 2;
      m_EnableCountDownThoughtLevels = true;

      // Don't limit high score
      m_HighestScore = -1;
   }
   else if (a_Personality == g_kHardAIName)
   {
      m_Personality = kAiPersonalityHard;
      m_MaxAIThoughtLevel = 2;
      m_EnableCountDownThoughtLevels = false;
   }

   m_CurrentAIThoughtLevel = m_MaxAIThoughtLevel;
}

cAiPersonality::~cAiPersonality()
{

}

std::vector<std::string> cAiPersonality::GetAINames()
{
   std::vector<std::string> l_ReturnVector;
   l_ReturnVector.push_back(g_kBeginnerAIName);
   l_ReturnVector.push_back(g_kBeginner2AIName);
   l_ReturnVector.push_back(g_kBeginner3AIName);
   l_ReturnVector.push_back(g_kEasyAIName);
   l_ReturnVector.push_back(g_kEasy2AIName);
   l_ReturnVector.push_back(g_kEasy3AIName);
   l_ReturnVector.push_back(g_kMediumAIName);
   l_ReturnVector.push_back(g_kMedium2AIName);
   l_ReturnVector.push_back(g_kMedium3AIName);
   l_ReturnVector.push_back(g_kHardAIName);


   return l_ReturnVector;
}

// Gets called when the AI starts thinking to allow parameters to change
// based on pressure.
void cAiPersonality::AdjustPersonalityToState(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
   sf::Vector2<uint32_t> a_FallingBean1,
   sf::Vector2<uint32_t> a_FallingBean2,
   uint32_t a_MiliSecPerFall
   )
{
   m_MiliSecPerFall = a_MiliSecPerFall;

   switch (m_Personality)
   {
      case kAiPersonalityBeginner:
      {
         _BeginnerPersonalityAdjustment(
            a_rPlayingField,
            a_FallingBean1,
            a_FallingBean2
            );
         break;
      }
      case kAiPersonalityBeginner2:
      {
         _Beginner2PersonalityAdjustment(
            a_rPlayingField,
            a_FallingBean1,
            a_FallingBean2
            );
         break;
      }
      case kAiPersonalityBeginner3:
      {
         _Beginner3PersonalityAdjustment(
            a_rPlayingField,
            a_FallingBean1,
            a_FallingBean2
            );
         break;
      }
      case kAiPersonalityEasy:
      {
         _EasyPersonalityAdjustment(
            a_rPlayingField,
            a_FallingBean1,
            a_FallingBean2
            );
         break;
      }
      case kAiPersonalityEasy2:
      case kAiPersonalityEasy3:
      case kAiPersonalityMedium:
      case kAiPersonalityMedium2:
      {
         _MediumPersonalityAdjustment(
            a_rPlayingField,
            a_FallingBean1,
            a_FallingBean2
            );
         break;
      }
      case kAiPersonalityMedium3:
      case kAiPersonalityHard:
      {
         _HardPersonalityAdjustment(
            a_rPlayingField,
            a_FallingBean1,
            a_FallingBean2
            );
         break;
      }
      default:
      {
         //~ std::cout << "Error: AI has unknown personality" << std::endl;
         _HardPersonalityAdjustment(
            a_rPlayingField,
            a_FallingBean1,
            a_FallingBean2
            );
         break;
      }
   }

   // Calculate m_CurrentDelayToFirstMove
   //~ std::random_device l_RandomSeeder;
   //~ uint32_t l_Seed = l_RandomSeeder();
   //~ std::minstd_rand l_Random(l_Seed);
   std::minstd_rand l_Random(std::chrono::high_resolution_clock::now().time_since_epoch().count());

   if (m_CurrentMinDelayToFirstMove >= m_CurrentMaxDelayToFirstMove)
   {
      m_CurrentMinDelayToFirstMove = m_CurrentMaxDelayToFirstMove;
      ++m_CurrentMaxDelayToFirstMove;
   }

   std::uniform_int_distribution<uint32_t> l_FirstMoveDistribution(
      m_CurrentMinDelayToFirstMove,
      //1000,
      //5000
      m_CurrentMaxDelayToFirstMove
      );

   m_CurrentDelayToFirstMove = l_FirstMoveDistribution(l_Random);

   //~ std::cout << "DELAY TO FIRST MOVE " << m_CurrentDelayToFirstMove << std::endl;

   // Calculate m_CurrentDelayToAdditionalMoves. First make sure
   // m_CurrentMaxDelayToAdditionalMoves isn't too small

   if (m_CurrentMaxDelayToAdditionalMoves < m_CurrentMinDelayToAdditionalMoves)
   {
      m_CurrentMaxDelayToAdditionalMoves = m_CurrentMinDelayToAdditionalMoves;
   }

   std::uniform_int_distribution<int> l_AdditionalMoveDistribution(
      m_CurrentMinDelayToAdditionalMoves,
      m_CurrentMaxDelayToAdditionalMoves
      );

   m_CurrentDelayToAdditionalMoves = l_AdditionalMoveDistribution(l_Random);

   std::uniform_int_distribution<int> l_FastFallDistribution(
      m_MinDelayToFastFall,
      m_MaxDelayToFastFall
      );
   m_CurrentDelayToFastFall = l_FastFallDistribution(l_Random);
}

// Lets the AI know that garbage fell in case it wants to adjust its strategy
void cAiPersonality::GarbageAdded()
{
   if (m_EnableCountDownThoughtLevels)
   {
      m_CurrentAIThoughtLevel = m_MaxAIThoughtLevel;
   }
}

uint32_t cAiPersonality::GetDelayToAdditionalMoves()
{
   return m_CurrentDelayToAdditionalMoves;
}

uint32_t cAiPersonality::GetDelayToFirstMove()
{

   return m_CurrentDelayToFirstMove;
}

uint32_t cAiPersonality::GetDelayToFastFall()
{
   return m_CurrentDelayToFastFall;
}

uint32_t cAiPersonality::GetAIThoughtLevel()
{
   return m_CurrentAIThoughtLevel;
}

uint32_t cAiPersonality::GetHighestScore()
{
   return m_HighestScore;
}

uint32_t cAiPersonality::GetOptimalMoveOdds()
{
   return m_OptimalMoveOdds;
}

bool cAiPersonality::IsRotationLocked()
{
   return m_LockRotation;
}

void cAiPersonality::_BeginnerPersonalityAdjustment(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
   sf::Vector2<uint32_t> a_FallingBean1,
   sf::Vector2<uint32_t> a_FallingBean2
   )
{
   uint32_t l_AlarmLevel = _AdjustAITiming(a_rPlayingField, a_FallingBean1, a_FallingBean2);

   // For every 140 points we can drop a garbage block. Limit the beginner AI to
   // only dropping up to 6.
   m_HighestScore = 6 * 140;
   m_CurrentAIThoughtLevel = 0;

   switch(l_AlarmLevel)
   {
      case 4:
      {
         m_OptimalMoveOdds = 0;
         break;
      }
      case 3:
      {
         m_OptimalMoveOdds = 20;
         break;
      }
      default:
      {
         m_OptimalMoveOdds = 35;
         break;
      }
   }

}

void cAiPersonality::_Beginner2PersonalityAdjustment(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
   sf::Vector2<uint32_t> a_FallingBean1,
   sf::Vector2<uint32_t> a_FallingBean2
   )
{

   uint32_t l_AlarmLevel = _AdjustAITiming(a_rPlayingField, a_FallingBean1, a_FallingBean2);

   // For every 140 points we can drop a garbage block. Limit the beginner AI to
   // only dropping up to 6.
   m_HighestScore = 12 * 140;
   m_LockRotation = false;

   switch(l_AlarmLevel)
   {
      case 4:
      {
         m_OptimalMoveOdds = 20;
         break;
      }
      case 3:
      {
         m_OptimalMoveOdds = 45;
         break;
      }
      case 0:
      {
         // This AI moves fast and dumb if it has a lot of breathing room
         m_LockRotation = true;
         m_OptimalMoveOdds = 20;
         // Allow connections but not full matches
         m_HighestScore = 4;
         m_MinDelayToFastFall = 40;
         m_MaxDelayToFastFall = 60;
         m_CurrentMaxDelayToFirstMove = 80;
         m_CurrentMaxDelayToAdditionalMoves = 50;
         m_CurrentMinDelayToAdditionalMoves = 30;
         break;
      }
      default:
      {
         m_OptimalMoveOdds = 60;
         break;
      }
   }
}

void cAiPersonality::_Beginner3PersonalityAdjustment(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
   sf::Vector2<uint32_t> a_FallingBean1,
   sf::Vector2<uint32_t> a_FallingBean2
   )
{
   uint32_t l_AlarmLevel = _AdjustAITiming(a_rPlayingField, a_FallingBean1, a_FallingBean2);

   // For every 140 points we can drop a garbage block. Limit the beginner AI to
   // only dropping up to 6.
   m_HighestScore = 14 * 140;

   switch(l_AlarmLevel)
   {
      case 4:
      {
         m_OptimalMoveOdds = 30;
         break;
      }
      case 3:
      {
         m_OptimalMoveOdds = 60;
         break;
      }
      default:
      {
         m_OptimalMoveOdds = 100;
         break;
      }
   }
}

void cAiPersonality::_EasyPersonalityAdjustment(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
   sf::Vector2<uint32_t> a_FallingBean1,
   sf::Vector2<uint32_t> a_FallingBean2
   )
{
   uint32_t l_AlarmLevel = _AdjustAITiming(a_rPlayingField, a_FallingBean1, a_FallingBean2);

   // For every 140 points we can drop a garbage block. Limit the beginner AI to
   // only dropping up to 6.
   m_HighestScore = 18 * 140;

   switch(l_AlarmLevel)
   {
      case 4:
      {
         m_OptimalMoveOdds = 60;
         break;
      }
      default:
      {
         m_OptimalMoveOdds = 100;
         break;
      }
   }
}

void cAiPersonality::_MediumPersonalityAdjustment(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
   sf::Vector2<uint32_t> a_FallingBean1,
   sf::Vector2<uint32_t> a_FallingBean2
   )
{
   uint32_t l_AlarmLevel = _AdjustAITiming(a_rPlayingField, a_FallingBean1, a_FallingBean2);

   switch(l_AlarmLevel)
   {
      case 4:
      case 3:
      {
         _AdjustThoughtLevel(0);
         break;
      }
      default:
      {
         _AdjustThoughtLevel(1);
         break;
      }
   }
}

void cAiPersonality::_HardPersonalityAdjustment(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
   sf::Vector2<uint32_t> a_FallingBean1,
   sf::Vector2<uint32_t> a_FallingBean2
   )
{
   uint32_t l_AlarmLevel = _AdjustAITiming(a_rPlayingField, a_FallingBean1, a_FallingBean2);

   // Hard AI needs time to think, so need to do some special adjustments
   if (  ColumnFreeSpace(a_rPlayingField, a_FallingBean1, a_FallingBean2) < 10
      && m_MiliSecPerFall < 50
      && l_AlarmLevel < 2
      )
   {
      l_AlarmLevel = 2;
   }

   switch(l_AlarmLevel)
   {
      case 4:
      case 3:
      case 2:
      {
         _AdjustThoughtLevel(1);
         break;
      }
      default:
      {
         _AdjustThoughtLevel(2);
         break;
      }
   }
}

void cAiPersonality::_AdjustThoughtLevel(uint32_t a_Max, uint32_t a_Min)
{
   if (m_EnableCountDownThoughtLevels)
   {
      if (m_CurrentAIThoughtLevel == a_Min)
      {
         m_CurrentAIThoughtLevel = a_Max;
      }
      else
      {
         --m_CurrentAIThoughtLevel;
      }
   }
   else
   {
      m_CurrentAIThoughtLevel = a_Max;
   }
}

uint32_t cAiPersonality::_AdjustAITiming(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
   sf::Vector2<uint32_t> a_FallingBean1,
   sf::Vector2<uint32_t> a_FallingBean2
   )
{
   // This is the game speed that the AI is comfortable with.
   uint32_t l_ComfortableGameSpeed =
      cSpeedController::GetStartingMsPerFallAtLevel(static_cast<uint32_t>(m_Personality));

   m_MinDelayToFastFall = l_ComfortableGameSpeed * 2;
   m_MaxDelayToFastFall = l_ComfortableGameSpeed * 4;

   uint32_t l_AlarmLevel = 0;
   if (ColumnFreeSpace(a_rPlayingField, a_FallingBean1, a_FallingBean2) < 3)
   {
      l_AlarmLevel = 4;
   }
   else
   {
      // No beans right under us, so base the pressure off of average height of the
      // columns. This average is actually the average number of empty spaces per
      // column.
      uint32_t l_Average = GetEmpySpaceAverage(a_rPlayingField);

      if (l_Average < (3 + 5) || ColumnFreeSpace(a_rPlayingField, a_FallingBean1, a_FallingBean2) < 5)
      {
         l_AlarmLevel = 3;
      }
      else if (l_Average < (5 + 5) || ColumnFreeSpace(a_rPlayingField, a_FallingBean1, a_FallingBean2) < 6)
      {
         l_AlarmLevel = 2;
      }
      else if (l_Average < (8 + 5))
      {
         l_AlarmLevel = 1;
      }
      else
      {
         l_AlarmLevel = 0;
      }
   }

   if (m_MiliSecPerFall < (l_ComfortableGameSpeed > 300 ? l_ComfortableGameSpeed - 300 : 0))
   {
      l_AlarmLevel += 4;
   }
   else if (m_MiliSecPerFall < (l_ComfortableGameSpeed > 200 ? l_ComfortableGameSpeed - 200 : 0))
   {
      l_AlarmLevel += 3;
   }
   else if (m_MiliSecPerFall < (l_ComfortableGameSpeed > 100 ? l_ComfortableGameSpeed - 100 : 0))
   {
      l_AlarmLevel += 2;
   }

   if (l_AlarmLevel > 4)
   {
      l_AlarmLevel = 4;
   }

   switch (l_AlarmLevel)
   {
      case 4:
      {
         //~ std::cout << "Very high pressure" << std::endl;
         m_CurrentMaxDelayToFirstMove = l_ComfortableGameSpeed / 2;
         m_CurrentMaxDelayToAdditionalMoves = l_ComfortableGameSpeed / 4;
         m_CurrentMinDelayToAdditionalMoves = 30 + (10 - static_cast<uint32_t>(m_Personality));
         break;
      }
      case 3:
      {
         //~ std::cout << "High Pressure" << std::endl;
         m_CurrentMaxDelayToFirstMove = l_ComfortableGameSpeed;
         m_CurrentMaxDelayToAdditionalMoves = l_ComfortableGameSpeed / 3;
         m_CurrentMinDelayToAdditionalMoves = 30 + (10 - static_cast<uint32_t>(m_Personality));
         break;
      }
      case 2:
      {
         //~ std::cout << "Med Pressure" << std::endl;
         // wait up to 1 bean falls before making the first move
         m_CurrentMaxDelayToFirstMove = l_ComfortableGameSpeed * 2;
         m_CurrentMaxDelayToAdditionalMoves = l_ComfortableGameSpeed / 2;
         m_CurrentMinDelayToAdditionalMoves = 30 + 2 * (10 - static_cast<uint32_t>(m_Personality));
         break;
      }
      // Not all AIs need this granulatiry. Case 0 is just for the AIs that want
      // to go crazy when there is low risk.
      case 1:
      case 0:
      {
         m_CurrentMaxDelayToFirstMove = l_ComfortableGameSpeed * 6;
         m_CurrentMaxDelayToAdditionalMoves = l_ComfortableGameSpeed;
         m_CurrentMinDelayToAdditionalMoves = 30 + 3 * (10 - static_cast<uint32_t>(m_Personality));
         break;
      }
      default:
      {
         // Shouldn't ever get here.
         m_CurrentMaxDelayToFirstMove = l_ComfortableGameSpeed * 6;
         m_CurrentMaxDelayToAdditionalMoves = l_ComfortableGameSpeed;
         m_CurrentMinDelayToAdditionalMoves = 30 + 3 * (10 - static_cast<uint32_t>(m_Personality));
         break;
      }
   }

   return l_AlarmLevel;
}

uint32_t ColumnFreeSpace(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
   sf::Vector2<uint32_t> a_FallingBean1,
   sf::Vector2<uint32_t> a_FallingBean2
   )
{
   uint32_t l_Column1 = a_FallingBean1.x;

   int32_t l_Min1 = 12;
   uint32_t i;
   for (i = 0; i < a_rPlayingField[l_Column1].size(); ++i)
   {
      if (a_rPlayingField[l_Column1][i].GetColor() != kBeanColorEmpty)
      {
         break;
      }
   }

   l_Min1 = i - static_cast<int32_t>(a_FallingBean1.y);

   uint32_t l_Column2 = a_FallingBean2.x;
   int32_t l_Min2 = 12;

   for (i = 0; i < a_rPlayingField[l_Column2].size(); ++i)
   {
      if (a_rPlayingField[l_Column2][i].GetColor() != kBeanColorEmpty)
      {
         break;
      }
   }

   l_Min2 = i - static_cast<int32_t>(a_FallingBean1.y);


   return std::min(l_Min1, l_Min2);
}

uint32_t GetEmpySpaceAverage(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField
   )
{
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

   return l_Average;
}
