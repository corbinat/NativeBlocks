#include "cLevel.h"
#include "cResources.h"
#include "cObject.h"
#include "cPhysFsInputStream.h"

#include <sstream>
#include <iostream>

// If frames go slower than this then loop the step several times. If frames go
// faster than this then wait.
static const uint32_t s_kFrameTimeInMs = 16;

cLevel::cLevel(
   cResources* a_pResources
   )
   : m_DebugMinFPS(100),
     m_DebugCounter(0),
     m_DebugString(),
     m_ObjectList(),
     m_EventList(),
     m_Running(true),
     m_InMemory(false),
     m_Clock(),
     m_StepSize(),
     m_FrameTimeAccumulatorMs(0),
     m_GridCellSize(32,32), // Default to 32x32 grid cells
     m_pResources(a_pResources),
     m_ClearColor(150,150,150,255) // Default to grey
{
   m_DebugString.setCharacterSize(15);

}

cLevel::~cLevel()
{
   // Remove objects
   // TODO: Assuming everything was allocated on the heap

   // We need to first set this level as the active level so that objects clean
   // up using the correct colision map.
   iLevelData * l_CurrentLevel = GetResources()->GetActiveLevelData();
   GetResources()->SetActiveLevelData(this);

   for (auto o = m_ObjectList.begin(); o != m_ObjectList.end(); ++o)
   {
      delete *o;
   }

   GetResources()->SetActiveLevelData(l_CurrentLevel);
}

bool cLevel::IsRunning()
{
   return m_Running;
}

void cLevel::UpdateObjects()
{
   if (!m_Running)
   {
      return;
   }

   std::list<cObject*>::iterator l_It;

   m_StepSize = m_Clock.restart();
   //~ m_FrameTimeAccumulatorMs += m_StepSize.asSeconds() * 1000 / 10;
   m_FrameTimeAccumulatorMs += m_StepSize.asSeconds() * 1000;
   //~ if (1/m_StepSize.asSeconds() < 40)
   //~ {
      //~ std::cout << "FPS: " << 1/m_StepSize.asSeconds() << std::endl;
   //~ }

   // Process Events
   sf::Event l_Event;
   while (m_pResources->GetWindow()->pollEvent(l_Event))
   {
      if (l_Event.type == sf::Event::Closed)
      {
         m_Running = false;
      }
      else
      {
         m_EventList.push_back(l_Event);
      }
   }

   //~ if (m_FrameTimeAccumulatorMs > s_kFrameTimeInMs * 2)
   //~ {
      //~ std::cout << "Stepping " << m_FrameTimeAccumulatorMs / s_kFrameTimeInMs << " Times" << std::endl;
   //~ }

   int l_StepCount = 0;
   while (m_FrameTimeAccumulatorMs >= s_kFrameTimeInMs)
   {
      ++l_StepCount;

      // Pass the events to objects. Start from back of the list because these
      // are the objects on top. They might want to prevent events from passed to
      // objects under them. This happens in the object update loop because
      // to be able to interpolate the drawing, we can only move objects in
      // deterministic frames.
      if (m_EventList.size() != 0)
      {
         for (std::list<cObject*>::reverse_iterator l_Rit = m_ObjectList.rbegin(); l_Rit != m_ObjectList.rend(); ++l_Rit)
         {
            (*l_Rit)->StartEvent(&m_EventList);
         }
      }
      m_EventList.clear();

      // Step the level
      Step(s_kFrameTimeInMs);

      // Step all objects
      std::list<cObject*> l_ObjectsToDestroy;
      for (l_It = m_ObjectList.begin(); l_It != m_ObjectList.end(); ++l_It)
      {
         if ((*l_It)->WaitingToBeDestroyed())
         {
            l_ObjectsToDestroy.push_back(*l_It);
         }
         else
         {
            (*l_It)->StartStep(s_kFrameTimeInMs);
         }
      }

      // Remove objects that were marked to be unregistered
      for (auto i = l_ObjectsToDestroy.begin(); i != l_ObjectsToDestroy.end(); ++i)
      {
         // Find i in the object list
         for (auto o = m_ObjectList.begin(); o != m_ObjectList.end(); ++o)
         {
            if (*o == *i)
            {
               delete *i;
               m_ObjectList.erase(o);
               break;
            }
         }
      }

      m_FrameTimeAccumulatorMs -= s_kFrameTimeInMs;
   }

   //~ if (l_StepCount > 1)
   //~ {
      //~ std::cout << "SLOWDOWN: Stepped " << l_StepCount << std::endl;
   //~ }
   // Draw all objects.

   m_pResources->GetWindow()->clear(m_ClearColor);
   for (l_It = m_ObjectList.begin(); l_It != m_ObjectList.end(); ++l_It)
   {
      (*l_It)->StartDraw(m_FrameTimeAccumulatorMs / static_cast<double>(s_kFrameTimeInMs));
   }

   // _DrawDebugOverlay(m_StepSize.asSeconds());

   m_pResources->GetWindow()->display();
}

bool cLevel::LoadLevelInMemory(bool a_Reload)
{
   if (m_InMemory && !a_Reload)
   {
      m_Clock.restart();
      return true;
   }

   // Remove any old objects
   // TODO: Assuming everything was allocated on the heap
   for (auto o = m_ObjectList.begin(); o != m_ObjectList.end(); ++o)
   {
      delete *o;
   }

   m_ObjectList.clear();

   Initialize();

   sf::Image l_ObjectMap;

   std::string l_ObjectMapPath = GetObjectMapPath();

   if (!l_ObjectMapPath.empty())
   {
      bool l_Status = LoadAsset(l_ObjectMap,GetObjectMapPath());

      if (!l_Status)
      {
         std::cout << "Fail" << std::endl;
         DebugPrintf("ERROR: Failed to load object map\n");
         return false;
      }

      sf::Vector2<uint32_t> l_Size = l_ObjectMap.getSize();
      for (uint32_t x = 0; x < l_Size.x; ++x)
      {
         for (uint32_t y = 0; y < l_Size.y; ++y)
         {
            sf::Color l_Pixel = l_ObjectMap.getPixel(x,y);
            cObject* l_pObject =
               PixelToObject(sf::Vector2<uint32_t>(x,y), l_Pixel, m_pResources);

            if (l_pObject != NULL)
            {
               l_pObject->Initialize();
            }
         }
      }
   }

   m_Clock.restart();

   m_InMemory = true;
   return true;
}

std::string cLevel::GetObjectMapPath()
{
   return "";
}

cResources* cLevel::GetResources()
{
   return m_pResources;
}

void cLevel::_DrawDebugOverlay(float a_StepSize)
{
   if ((1/a_StepSize) < m_DebugMinFPS && m_DebugCounter > 99)
   {
      m_DebugMinFPS = 1 / a_StepSize;
   }

   if (m_DebugCounter < 200)
   {
      ++m_DebugCounter;
   }
   else
   {

      std::ostringstream l_FrameRateStream;
      l_FrameRateStream << "FPS: ";
      if (a_StepSize > 0)
      {
         l_FrameRateStream << (1 / a_StepSize);
      }
      else
      {
         l_FrameRateStream << "Inf";
      }

      l_FrameRateStream << "\t Min: " << m_DebugMinFPS;

      l_FrameRateStream << "\t Objects: " << m_ObjectList.size();

      m_DebugString.setString(l_FrameRateStream.str());
      m_DebugCounter = 100;
   }

   m_pResources->GetWindow()->draw(m_DebugString);

}


void cLevel::Step(uint32_t a_ElapsedMiliSec)
{
}

void cLevel::Initialize()
{
}

void cLevel::SetGridCellSize(sf::Vector2<uint32_t> a_GridSize)
{
   m_GridCellSize = a_GridSize;
}

void cLevel::SetBackgroundColor(sf::Color a_Color)
{
   m_ClearColor = a_Color;
}

cObject* cLevel::GetObjectWithId(int32_t a_Id)
{
   for (auto i = m_ObjectList.begin(); i != m_ObjectList.end(); ++i)
   {
      if ((*i)->GetUniqueId() == a_Id)
      {
         return *i;
      }
   }
   return NULL;
}

std::list<cObject*>* cLevel::GetObjectList()
{
   return &m_ObjectList;
}

std::map<std::pair<int32_t, int32_t>, std::list<cObject*>>* cLevel::GetCollisionMap()
{
   return &m_CollisionMap;
}

// Level grid size. Effects granularity of collision map
sf::Vector2<uint32_t>* cLevel::GetGridCellSize()
{
   return &m_GridCellSize;
}
