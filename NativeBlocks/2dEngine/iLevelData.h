// The purpose of this class is for cObjects to be able to retrieve information
// about the current level.

#ifndef ___iLevel_h___
#define ___iLevel_h___

#include "SFML/System.hpp"

#include <stdint.h> // uint32_t
#include <string>
#include <list>
#include <map>
#include <utility>   // std::pair

class cObject;

class iLevelData
{
public:
   iLevelData()
   {
   }

   virtual ~iLevelData()
   {
   }

   virtual cObject* GetObjectWithId(int32_t a_Id) = 0;

   virtual std::list<cObject*>* GetObjectList() = 0;

   virtual std::map<std::pair<int32_t, int32_t>, std::list<cObject*>>* GetCollisionMap() = 0;

   // Level grid size. Effects granularity of collision map
   virtual sf::Vector2<uint32_t>* GetGridCellSize() = 0;


};

#endif
