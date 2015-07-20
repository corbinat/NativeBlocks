// This object observes a player

#ifndef ___cStaging_h___
#define ___cStaging_h___

#include "cBean.h"

class cResources;

class cStaging
{
public:
   cStaging(
      cResources* a_pResources,
      std::minstd_rand a_RandomNumberEngine,
      uint32_t a_PlayerId,
      size_t a_BeanPoolSize
      );

   ~cStaging();

   cBean* GetGarbageBean();
   cBean* GetNextBean();

   // Position is 0 indexed where 0 is the next bean in the staging area
   eBeanColor InspectNextBeanColor(uint32_t a_Position);


private:

   eBeanColor _NumberToColor(uint32_t a_Number);

   std::minstd_rand m_RandomNumberEngine;

   cResources* m_pResources;

   uint32_t m_PlayerId;

   size_t m_BeanPoolSize;
   std::vector<cBean*> m_BeenPool;

};

#endif
