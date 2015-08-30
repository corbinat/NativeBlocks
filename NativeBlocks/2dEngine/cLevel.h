#ifndef ___cLevel_h___
#define ___cLevel_h___

#include "Common.h"
#include "cVFileNode.h"
#include "iLevelData.h"

// SFML
#include "SFML/Graphics.hpp"

#include <stdint.h> // uint32_t
#include <string>
#include <list>
#include <map>
#include <utility>   // std::pair

class cObject;
class cResources;

class cLevel : public iLevelData
{
public:
   cLevel(
      cResources* a_pResources
      );

   virtual ~cLevel();

   bool IsRunning();

   void UpdateObjects();

   bool LoadLevelInMemory(bool a_Reload);

   virtual std::string GetLevelName() = 0;
   virtual std::string GetObjectMapPath();

   cResources* GetResources();

   // iLevelData
   cObject* GetObjectWithId(int32_t a_Id);

   std::list<cObject*>* GetObjectList();

   std::map<std::pair<int32_t, int32_t>, std::list<cObject*>>* GetCollisionMap();

   // Level grid size. Effects granularity of collision map
   sf::Vector2<uint32_t>* GetGridCellSize();

protected:
   // The object locations will be defined by a bmp file where a color maps to
   // a certain object. Users should overload this function with a switch
   // statement mapping colors to a certain class of objects. For example,
   // return new cMyObject. Position is also included in case other initial
   // settings should be set.
   virtual cObject* PixelToObject(
      sf::Vector2<uint32_t> a_position,
      sf::Color a_Color,
      cResources* a_pResources
      )
   {
      return NULL;
   }

   virtual void PixelToTile() {}

   virtual void Step (uint32_t a_ElapsedMiliSec);

   // To be called whenever the level is reloaded
   virtual void Initialize();

   // These can be called by derived classes to change defaults
   virtual void SetGridCellSize(sf::Vector2<uint32_t> a_GridSize);
   virtual void SetBackgroundColor(sf::Color a_Color);

private:

   // Debugging overlay
   void _DrawDebugOverlay(float a_StepSize);
   float m_DebugMinFPS;
   uint32_t m_DebugCounter;
   sf::Text m_DebugString;

   // Objects in this level
   std::map<std::pair<int32_t, int32_t>, std::list<cObject*>> m_CollisionMap;

   // This has to be a list to prevent iterators from getting wonky if we add
   // elements while stepping through the list
   std::list<cObject*> m_ObjectList;

   std::list<sf::Event> m_EventList;

   // Used to determine if the window should stay open
   bool m_Running;

   // Used to determine if the objects for this level have already been created
   bool m_InMemory;

   // Used to determine step sizes
   sf::Clock m_Clock;
   sf::Time m_StepSize;
   double m_FrameTimeAccumulatorMs;

   // Level grid size. Effects granularity of collision map
   sf::Vector2<uint32_t> m_GridCellSize;

   cResources* m_pResources;

   sf::Color m_ClearColor;

};

#endif
