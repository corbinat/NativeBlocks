#include "cStaging.h"

#include "cBean.h"
#include "cResources.h"

cStaging::cStaging(
   cResources* a_pResources,
   std::minstd_rand a_RandomNumberEngine,
   uint32_t a_PlayerId
   )
   : m_pResources(a_pResources),
     m_RandomNumberEngine(),
     m_PlayerId(a_PlayerId)
{
   m_RandomNumberEngine = a_RandomNumberEngine;
}

cStaging::~cStaging()
{

}

cBean* cStaging::GetNextBean()
{
   // Advance the random number engine
   m_RandomNumberEngine();
   std::uniform_int_distribution<int> l_Distribution(0, kBeanColorNumber - 1);
   //std::uniform_int_distribution<int> l_Distribution(0, kBeanColorNumber - 4);

   int l_Number = l_Distribution(m_RandomNumberEngine);
   l_Number = l_Distribution(m_RandomNumberEngine);
   eBeanColor l_Color = _NumberToColor(l_Number);

   cBean * a_pBean = new cBean(l_Color, m_pResources, m_PlayerId);

   return a_pBean;
}

eBeanColor cStaging::InspectNextBeanColor(uint32_t a_Position)
{
   // Copy the number engine so we don't advance the real thing
   std::minstd_rand l_Engine = m_RandomNumberEngine;

   for (int32_t i = 0; i < a_Position - 1; ++i)
   {
      l_Engine();
   }

   std::uniform_int_distribution<int> l_Distribution(0, kBeanColorNumber - 1);
   //std::uniform_int_distribution<int> l_Distribution(0, kBeanColorNumber - 4);

   int l_Number = l_Distribution(l_Engine);
   l_Number = l_Distribution(l_Engine);

   return _NumberToColor(l_Number);
}


eBeanColor cStaging::_NumberToColor(uint32_t a_Number)
{
   switch(a_Number)
   {
      case 0:
      {
         return kBeanColorBlue;
      }
      case 1:
      {
         return kBeanColorGreen;
      }
      case 2:
      {
         return kBeanColorYellow;
      }
      case 3:
      {
         return kBeanColorOrange;
      }
      case 4:
      {
         return kBeanColorPink;
      }
      default:
      {
         return kBeanColorGarbage;
      }
   }
}
