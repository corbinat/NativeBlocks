#ifndef ___cLevelManager_h___
#define ___cLevelManager_h___

#include "cLevel.h"
#include <vector>

class cLevelManager
{
public:
   cLevelManager(cResources* a_pResources);
   ~cLevelManager();

   void AddLevel(cLevel* a_pLevel);
   void Update();

   bool IsRunning();

private:

   cResources* m_pResources;
   std::vector<cLevel*> m_Levels;
   std::vector<cLevel*>::iterator m_CurrentLevel;

   bool m_Running;
   bool m_IsEmpty;

};

#endif
