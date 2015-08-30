#include "cLevelManager.h"
#include "cResources.h"

cLevelManager::cLevelManager(cResources* a_pResources)
   : m_pResources(a_pResources),
     m_Levels(),
     m_CurrentLevel(m_Levels.end()),
     m_Running(true),
     m_IsEmpty(true)
{
}

cLevelManager::~cLevelManager()
{
}

void cLevelManager::AddLevel(cLevel* a_pLevel)
{
   m_Levels.push_back(a_pLevel);
}

void cLevelManager::Update()
{
   bool l_LevelNeedsReload = m_pResources->ActiveLevelNeedsReload();

   if (  (m_IsEmpty)
      || (m_pResources->GetActiveLevel() != (*m_CurrentLevel)->GetLevelName())
      || l_LevelNeedsReload
      )
   {
      // Find the new level
      for (auto i = m_Levels.begin(); i != m_Levels.end(); ++i)
      {
         if ((*i)->GetLevelName() == m_pResources->GetActiveLevel())
         {
            m_CurrentLevel = i;

            // Reset the view
            m_pResources->GetWindow()->setView(
               m_pResources->GetWindow()->getDefaultView()
               );

            m_pResources->SetActiveLevelData(*m_CurrentLevel);
            (*m_CurrentLevel)->LoadLevelInMemory(l_LevelNeedsReload);
            m_IsEmpty = false;

            break;
         }
      }
   }

   if (m_CurrentLevel != m_Levels.end())
   {
      if (!(*m_CurrentLevel)->IsRunning())
      {
         m_Running = false;
      }
      (*m_CurrentLevel)->UpdateObjects();
   }
   else
   {
      m_Running = false;
   }

   if (!m_Running)
   {
      if (m_pResources->GetWindow()->isOpen())
      {
         m_pResources->GetWindow()->close();
      }
   }
}

bool cLevelManager::IsRunning()
{
   return m_Running;
}
