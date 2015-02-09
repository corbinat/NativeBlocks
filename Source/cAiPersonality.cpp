#include "cAiPersonality.h"

#include <iostream>

const uint32_t g_kMinFirstDelayDefault = 30;
const uint32_t g_kMinAdditionalDelayDefault = 50;

cAiPersonality::cAiPersonality(eAiPersonality a_Personality)
   : m_MaxAIThoughtLevel(0),
     m_CurrentAIThoughtLevel(0),
     m_EnableCountDownThoughtLevels(false),
     m_CurrentMinDelayToFirstMove(30),
     m_CurrentMaxDelayToFirstMove(0),
     m_CurrentDelayToFirstMove(0),
     m_CurrentMinDelayToAdditionalMoves(50),
     m_CurrentMaxDelayToAdditionalMoves(0),
     m_CurrentDelayToAdditionalMoves(0),
     m_HighestScore(0),
     m_Personality(a_Personality),
     m_MiliSecPerFall(100)
{
   switch(m_Personality)
   {
      case kAiPersonalityBeginner:
      {
         m_MaxAIThoughtLevel = 0;
         m_EnableCountDownThoughtLevels = false;
         m_HighestScore = 0;
         break;
      }
      case kAiPersonalityEasy:
      {
         m_MaxAIThoughtLevel = 0;
         m_EnableCountDownThoughtLevels = false;
         m_HighestScore = 0;
         break;
      }
      case kAiPersonalityMedium:
      {
         m_MaxAIThoughtLevel = 1;
         m_EnableCountDownThoughtLevels = false;
         m_HighestScore = 0;
         break;
      }
      case kAiPersonalityHard:
      {
         m_MaxAIThoughtLevel = 2;
         m_EnableCountDownThoughtLevels = false;
         m_HighestScore = 0;
         break;
      }
      default:
      {
         //~ std::cout << "Error: AI has unknown personality" << std::endl;
         break;
      }
   }

   m_CurrentAIThoughtLevel = m_MaxAIThoughtLevel;
}

cAiPersonality::~cAiPersonality()
{

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
         std::cout << "Beginer thinking..." << std::endl;
         _BeginnerPersonalityAdjustment(
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
      case kAiPersonalityMedium:
      {
         _MediumPersonalityAdjustment(
            a_rPlayingField,
            a_FallingBean1,
            a_FallingBean2
            );
         break;
      }
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
   std::random_device l_RandomSeeder;
   uint32_t l_Seed = l_RandomSeeder();
   std::minstd_rand l_Random(l_Seed);

   //~ std::cout << "RANDOM: " << m_MinDelayToFirstMove << "," << m_CurrentMaxDelayToFirstMove << std::endl;

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

}

// Lets the AI know that garbage fell in case it wants to adjust its strategy
void cAiPersonality::GarbageAdded()
{

}

uint32_t cAiPersonality::GetDelayToAdditionalMoves()
{
   return m_CurrentDelayToAdditionalMoves;
}

uint32_t cAiPersonality::GetDelayToFirstMove()
{

   return m_CurrentDelayToFirstMove;
}

uint32_t cAiPersonality::GetAIThoughtLevel()
{
   return m_CurrentAIThoughtLevel;
}

uint32_t cAiPersonality::GetHighestScore()
{
   return m_HighestScore;
}

void cAiPersonality::_BeginnerPersonalityAdjustment(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
   sf::Vector2<uint32_t> a_FallingBean1,
   sf::Vector2<uint32_t> a_FallingBean2
   )
{
   m_CurrentAIThoughtLevel = 0;

   // For every 140 points we can drop a garbage block. Limit the beginner AI to
   // only dropping up to 6.
   m_HighestScore = 6 * 140;

   if (IsColumnUrgencyHigh(a_rPlayingField, a_FallingBean1, a_FallingBean2))
   {
      //~ std::cout << "Very high pressure" << std::endl;
      m_CurrentMaxDelayToFirstMove = m_MiliSecPerFall / 4;
      m_CurrentMaxDelayToAdditionalMoves = m_MiliSecPerFall / 6;
      m_CurrentMinDelayToAdditionalMoves = g_kMinAdditionalDelayDefault * 1.2;
   }
   else
   {
      // No beans right under us, so base the pressure off of average height of the
      // columns. This average is actually the average number of empty spaces per
      // column.
      uint32_t l_Average = GetEmpySpaceAverage(a_rPlayingField);

      // If bean level is high then the pressure is high. Add 5 to account for
      // garbage rows
      if (l_Average < (2 + 5))
      {
         //~ std::cout << "High Pressure" << std::endl;
         m_CurrentMaxDelayToFirstMove = m_MiliSecPerFall;
         m_CurrentMaxDelayToAdditionalMoves = m_MiliSecPerFall / 3;
         m_CurrentMinDelayToAdditionalMoves = g_kMinAdditionalDelayDefault * 1.2;

      }
      // If bean level is midway then the pressure is up a just a bit. Add 5 to
      // account for garbage rows
      else if (l_Average < (4 + 5))
      {
         //~ std::cout << "Med Pressure" << std::endl;
         // wait up to 1 bean falls before making the first move
         m_CurrentMaxDelayToFirstMove = m_MiliSecPerFall * 2;
         m_CurrentMaxDelayToAdditionalMoves = m_MiliSecPerFall / 2;
         m_CurrentMinDelayToAdditionalMoves = g_kMinAdditionalDelayDefault * 1.5;
      }
      else
      {
         // wait up to 6 bean falls before making the first move
         m_CurrentMaxDelayToFirstMove = m_MiliSecPerFall * 6;
         m_CurrentMaxDelayToAdditionalMoves = m_MiliSecPerFall;
         m_CurrentMinDelayToAdditionalMoves = g_kMinAdditionalDelayDefault * 2;
      }
   }
}
void cAiPersonality::_EasyPersonalityAdjustment(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
   sf::Vector2<uint32_t> a_FallingBean1,
   sf::Vector2<uint32_t> a_FallingBean2
   )
{
   m_CurrentAIThoughtLevel = 0;

   if (IsColumnUrgencyHigh(a_rPlayingField, a_FallingBean1, a_FallingBean2))
   {
      //~ std::cout << "Very high pressure" << std::endl;
      m_CurrentMaxDelayToFirstMove = m_MiliSecPerFall / 4;
      m_CurrentMaxDelayToAdditionalMoves = m_MiliSecPerFall / 6;
   }
   else
   {
      // No beans right under us, so base the pressure off of average height of the
      // columns. This average is actually the average number of empty spaces per
      // column.
      uint32_t l_Average = GetEmpySpaceAverage(a_rPlayingField);

      // If bean level is high then the pressure is high. Add 5 to account for
      // garbage rows
      if (l_Average < (3 + 5))
      {
         //~ std::cout << "High Pressure" << std::endl;
         m_CurrentMaxDelayToFirstMove = m_MiliSecPerFall / 2;
         m_CurrentMaxDelayToAdditionalMoves = m_MiliSecPerFall / 3;

      }
      // If bean level is midway then the pressure is up a just a bit. Add 5 to
      // account for garbage rows
      else if (l_Average < (4 + 5))
      {
         //~ std::cout << "Med Pressure" << std::endl;
         // wait up to 1 bean falls before making the first move
         m_CurrentMaxDelayToFirstMove = m_MiliSecPerFall;
         m_CurrentMaxDelayToAdditionalMoves = m_MiliSecPerFall / 3;
      }
      else
      {
         // wait up to 3 bean falls before making the first move
         m_CurrentMaxDelayToFirstMove = m_MiliSecPerFall * 3;
         m_CurrentMaxDelayToAdditionalMoves = m_MiliSecPerFall / 2;
      }
   }
}

void cAiPersonality::_MediumPersonalityAdjustment(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
   sf::Vector2<uint32_t> a_FallingBean1,
   sf::Vector2<uint32_t> a_FallingBean2
   )
{
   m_CurrentAIThoughtLevel = 1;

   if (IsColumnUrgencyHigh(a_rPlayingField, a_FallingBean1, a_FallingBean2))
   {
      //~ std::cout << "Very high pressure" << std::endl;
      m_CurrentMaxDelayToFirstMove = m_MiliSecPerFall / 4;
      m_CurrentMaxDelayToAdditionalMoves = m_MiliSecPerFall / 6;
      m_CurrentAIThoughtLevel = 0;
   }
   else
   {
      // No beans right under us, so base the pressure off of average height of the
      // columns. This average is actually the average number of empty spaces per
      // column.
      uint32_t l_Average = GetEmpySpaceAverage(a_rPlayingField);

      // If bean level is high then the pressure is high. Add 5 to account for
      // garbage rows
      if (l_Average < (3 + 5))
      {
         //~ std::cout << "High Pressure" << std::endl;
         m_CurrentMaxDelayToFirstMove = m_MiliSecPerFall / 4;
         m_CurrentMaxDelayToAdditionalMoves = m_MiliSecPerFall / 6;
         m_CurrentAIThoughtLevel = 0;

      }
      // If bean level is midway then the pressure is up a just a bit. Add 5 to
      // account for garbage rows
      else if (l_Average < (4 + 5))
      {
         //~ std::cout << "Med Pressure" << std::endl;
         // wait up to 1 bean falls before making the first move
         m_CurrentMaxDelayToFirstMove = m_MiliSecPerFall / 2;
         m_CurrentMaxDelayToAdditionalMoves = m_MiliSecPerFall / 4;
      }
      else
      {
         // wait up to 2 bean falls before making the first move
         m_CurrentMaxDelayToFirstMove = m_MiliSecPerFall * 2;
         m_CurrentMaxDelayToAdditionalMoves = m_MiliSecPerFall / 2;

         //~ std::cout << "WHAT IS TIHS: " << m_CurrentMaxDelayToFirstMove << std::endl;
      }
   }
}

void cAiPersonality::_HardPersonalityAdjustment(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
   sf::Vector2<uint32_t> a_FallingBean1,
   sf::Vector2<uint32_t> a_FallingBean2
   )
{
   m_CurrentAIThoughtLevel = 2;

   if (IsColumnUrgencyHigh(a_rPlayingField, a_FallingBean1, a_FallingBean2))
   {
      //~ std::cout << "Very high pressure" << std::endl;
      m_CurrentMaxDelayToFirstMove = m_MiliSecPerFall / 4;
      m_CurrentMaxDelayToAdditionalMoves = m_MiliSecPerFall / 6;
      m_CurrentAIThoughtLevel = 1;
   }
   else
   {
      // No beans right under us, so base the pressure off of average height of the
      // columns. This average is actually the average number of empty spaces per
      // column.
      uint32_t l_Average = GetEmpySpaceAverage(a_rPlayingField);

      // If bean level is high then the pressure is high. Add 5 to account for
      // garbage rows
      if (l_Average < (3 + 5))
      {
         //~ std::cout << "High Pressure" << std::endl;
         m_CurrentMaxDelayToFirstMove = m_MiliSecPerFall / 4;
         m_CurrentMaxDelayToAdditionalMoves = m_MiliSecPerFall / 6;
         m_CurrentAIThoughtLevel = 1;

      }
      // If bean level is midway then the pressure is up a just a bit. Add 5 to
      // account for garbage rows
      else if (l_Average < (4 + 5))
      {
         //~ std::cout << "Med Pressure" << std::endl;
         // wait up to 1 bean falls before making the first move
         m_CurrentMaxDelayToFirstMove = m_MiliSecPerFall / 2;
         m_CurrentMaxDelayToAdditionalMoves = m_MiliSecPerFall / 4;
         m_CurrentAIThoughtLevel = 1;
      }
      else
      {
         // wait up to 2 bean falls before making the first move
         m_CurrentMaxDelayToFirstMove = m_MiliSecPerFall;
         m_CurrentMaxDelayToAdditionalMoves = m_MiliSecPerFall / 2;
      }
   }
}

bool IsColumnUrgencyHigh(
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
         <= 2
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
         <= 2
         )
      {
         return true;
      }
   }

   return false;
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
