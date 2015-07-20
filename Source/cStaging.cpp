#include "cStaging.h"

#include "cBean.h"
#include "cResources.h"

cStaging::cStaging(
   cResources* a_pResources,
   std::minstd_rand a_RandomNumberEngine,
   uint32_t a_PlayerId,
   size_t a_BeanPoolSize
   )
   : m_pResources(a_pResources),
     m_RandomNumberEngine(),
     m_PlayerId(a_PlayerId),
     m_BeanPoolSize(a_BeanPoolSize),
     m_BeenPool()
{
   m_RandomNumberEngine = a_RandomNumberEngine;

   // We can have 6 * 12 beans in the playing area plus 6 * 5 garbage beans
   // queued up. We'll do 110 just in case;
   for (int i = 0; i < m_BeanPoolSize; ++i)
   {
      m_BeenPool.push_back(new cBean(kBeanColorBlue, m_pResources, m_PlayerId));
   }
}

cStaging::~cStaging()
{

}

cBean* cStaging::GetGarbageBean()
{
   // Find an unused bean in the BeanPool
   cBean* l_AvailableBean = NULL;

   for (auto l_Bean : m_BeenPool)
   {
      if (!(l_Bean->IsVisible()))
      {
         l_Bean->ResetBean();
         l_AvailableBean = l_Bean;
         break;
      }
   }

   if (l_AvailableBean == NULL)
   {
      // This shouldn't ever happen
      std::cout << "Couldn't find an available Bean!!!!!" << std::endl;
      l_AvailableBean = new cBean(kBeanColorGarbage, m_pResources, m_PlayerId);
      m_BeenPool.push_back(l_AvailableBean);
   }

   l_AvailableBean->SetColor(kBeanColorGarbage);

   return l_AvailableBean;
}

cBean* cStaging::GetNextBean()
{
   // Find an unused bean in the BeanPool
   cBean* l_AvailableBean = NULL;

   for (auto l_Bean : m_BeenPool)
   {
      if (!(l_Bean->IsVisible()))
      {
         l_Bean->ResetBean();
         l_AvailableBean = l_Bean;
         break;
      }
   }

   // Advance the random number engine
   std::uniform_int_distribution<int> l_Distribution(0, kBeanColorNumber - 1);

   int l_Number = l_Distribution(m_RandomNumberEngine);
   eBeanColor l_Color = _NumberToColor(l_Number);

   if (l_AvailableBean == NULL)
   {
      // This shouldn't ever happen
      std::cout << "Couldn't find an available Bean!!!!!" << std::endl;
      l_AvailableBean = new cBean(l_Color, m_pResources, m_PlayerId);
      m_BeenPool.push_back(l_AvailableBean);
   }

   l_AvailableBean->SetColor(l_Color);

   return l_AvailableBean;
}

eBeanColor cStaging::InspectNextBeanColor(uint32_t a_Position)
{
   // Copy the number engine so we don't advance the real thing
   std::minstd_rand l_Engine = m_RandomNumberEngine;

   for (int32_t i = 0; i < a_Position; ++i)
   {
      l_Engine();
   }

   std::uniform_int_distribution<int> l_Distribution(0, kBeanColorNumber - 1);

   int l_Number = l_Distribution(l_Engine);

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
