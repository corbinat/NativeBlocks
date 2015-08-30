#include "cObject.h"
#include "CollisionHelpers.h"
#include "cResources.h"
#include "cSound.h"

//testing
#include "cVFileNode.h"
#include <iostream>
//

#include <fstream>
#include <cmath>

cObject::cObject(cResources* a_pResources)
   : m_ZSize(0),
     m_pResources(a_pResources),
     m_pView(NULL),
     m_UniqueId(a_pResources->GenerateNewUniqueId()),
     m_Solid(false),
     m_Collidable(false),
     m_Type("cObject"),
     m_Disabled(false),
     m_ToBeDestroyed(false),
     m_Persistent(false),
     m_AniManager(a_pResources),
     m_Visible(true),
     m_PreviousFramePosition(),
     m_Position(0,0,0),
     m_PreviousPosition(0,0,0),
     m_Velocity(0,0,0),
     m_AdjustedVelocity(0,0,0),
     m_Depth(0),
     m_ChildObjects(),
     m_pParentObject(NULL)
{
   //DebugTrace();
   std::list<cObject*> * l_pObjectList =
      GetResources()->GetActiveLevelData()->GetObjectList();

   l_pObjectList->push_back(this);
   l_pObjectList->sort(cObject::ObjectOrder);

   AddToCollisionMap();
}

cObject::~cObject()
{
   //DebugTrace();
   Disable(true);

   SetParent(NULL);
   RemoveAllChildren();
}

void cObject::Initialize()
{

}

void cObject::StartStep (uint32_t a_ElapsedMiliSec)
{
   if (m_Disabled)
   {
      return;
   }

   if (m_PreviousPosition == GetPosition())
   {
      // Other objects haven't moved us, so this is safe.
      m_PreviousFramePosition = GetPosition();
   }

   double l_Seconds = a_ElapsedMiliSec/1000.0;
   m_AdjustedVelocity = GetVelocity() * l_Seconds;
   SetPosition(m_AdjustedVelocity, kRelative);

   m_AniManager.StepAnimation(a_ElapsedMiliSec);

   Step(a_ElapsedMiliSec);

   // If the object moved then update the sprite and notify other objects
   if (m_PreviousPosition != m_Position)
   {
      // Update the collision map
      RemoveFromCollisionMap(true);
      AddToCollisionMap();

      // Update the sprite's position too
      m_AniManager.SetPosition(m_Position.x, m_Position.y);

      // Update view
      if (m_pView != NULL)
      {
         m_pView->Update(GetBoundingVerticies());
         m_pResources->GetWindow()->setView(*m_pView);
      }

      // This needs to be at the bottom because it needs to be set right after
      // updating the collision map. If it is at the top then we will overwrite
      // the previous position before updating the collision map and therefore
      // corrupt it if other objects moved this one.
      m_PreviousPosition = m_Position;

   }
}
void cObject::Step (uint32_t a_ElapsedMiliSec)
{
}

void cObject::StartDraw(double a_InterpolateFactor)
{
   if (m_Disabled || !m_Visible)
   {
      return;
   }

   // Drawing one frame behind. This allows us to interpolate
   sf::Vector3<double> l_PreviousPosition = m_PreviousFramePosition;
   l_PreviousPosition += (GetPosition() - m_PreviousFramePosition) * a_InterpolateFactor;

   sf::Vector2<float> l_SpritePosition;
   l_SpritePosition.x = static_cast<float>(l_PreviousPosition.x);
   l_SpritePosition.y = static_cast<float>(l_PreviousPosition.y);

   sf::Vector2<float> l_SpriteDestination;
   l_SpriteDestination.x = static_cast<float>(GetPosition().x);
   l_SpriteDestination.y = static_cast<float>(GetPosition().y);

   sf::Sprite* l_Sprite = GetSprite();

   if (l_Sprite != NULL)
   {
      //~ if (GetType() == "Button")
      //~ {
         //~ std::cout << "Sprite original: " << m_PreviousFramePosition.x << std::endl;
         //~ std::cout << "Sprite position: " << l_SpritePosition.x << std::endl;
         //~ std::cout << "Sprite Destination: " << GetPosition().x << std::endl;
      //~ }
      l_Sprite->setPosition(l_SpritePosition);
      m_pResources->GetWindow()->draw(*l_Sprite);
   }

   // Give objects an offset of where they should draw.
   Draw(l_SpritePosition - l_SpriteDestination);

   return;
}

void cObject::Draw(const sf::Vector2<float> & a_rkOffset)
{
}

void cObject::StartEvent (std::list<sf::Event> * a_pEventList)
{
   if (m_Disabled)
   {
      return;
   }
   Event(a_pEventList);
}

void cObject::Event (std::list<sf::Event> * a_pEventList)
{
}

void cObject::Collision(cObject* a_pOther)
{
}

///////////////////////////////////////////////////////////////////////
//
// General Properties
//
//////////////////////////////////////////////////////////////////////

int32_t cObject::GetUniqueId()
{
   return m_UniqueId;
}

bool cObject::IsSolid()
{
   return m_Solid;
}

void cObject::SetSolid(bool a_IsSolid)
{
   m_Solid = a_IsSolid;
}

bool cObject::IsCollidable()
{
   return m_Collidable;
}

void cObject::SetCollidable(bool a_IsCollidable)
{
   if (m_Disabled)
   {
      return;
   }

   if (!m_Collidable && a_IsCollidable)
   {
      AddToCollisionMap();
   }
   else if (m_Collidable && !a_IsCollidable)
   {
      RemoveFromCollisionMap(false);
   }

   m_Collidable = a_IsCollidable;
}

void cObject::SetType(std::string a_Type)
{
   m_Type = a_Type;
}

std::string cObject::GetType(void)
{
   return m_Type;
}

void cObject::Disable(bool a_Disable)
{
   m_Disabled = a_Disable;

   if (m_Disabled)
   {
      RemoveFromCollisionMap(false);
   }
   else
   {
      AddToCollisionMap();
   }
}

bool cObject::Disabled()
{
   return m_Disabled;
}

bool cObject::WaitingToBeDestroyed()
{
   return m_ToBeDestroyed;
}

void cObject::Persistent(bool a_Persistent)
{
   m_Persistent = a_Persistent;
}

bool cObject::Persistent()
{
   return m_Persistent;
}

void cObject::UnregisterObject(bool a_Delete)
{
   Disable(true);

   m_ToBeDestroyed = true;
}

bool cObject::ObjectOrder(cObject* a_ObjectA, cObject* a_ObjectB)
{
   return (a_ObjectA->GetDepth() > a_ObjectB->GetDepth());
}

void cObject::SetView(cView* a_pView)
{
   m_pView = a_pView;

   // Update view
   if (m_pView != NULL)
   {
      m_pView->Update(GetBoundingVerticies());
      m_pResources->GetWindow()->setView(*m_pView);
   }
}

cResources* cObject::GetResources()
{
   return m_pResources;
}

///////////////////////////////////////////////////////////////////////
//
// Graphics
//
//////////////////////////////////////////////////////////////////////

// Load the Sprites and animations from an animation descriptor text file.
bool cObject::LoadAnimations(std::string a_FileName)
{
   bool l_Success = m_AniManager.LoadAnimations(a_FileName);

   if (l_Success)
   {
      // Update the sprite's position too
      m_AniManager.SetPosition(m_Position.x, m_Position.y);
   }

   return l_Success;
}

// Set the animation to something that loops (such as running). This can
// double as a static sprite if the animation only has 1 frame.
void cObject::PlayAnimationLoop(std::string a_Name)
{
   m_AniManager.PlayAnimationLoop(a_Name);
}

// Play an animation that does not loop (such as jumping). When this
// animation is done the previous static sprite or looping animation should
// start again.
void cObject::PlayAnimationOnce(
   std::string a_Name,
   std::function<void(void)> a_Callback
   )
{
   //DebugTrace();
   m_AniManager.PlayAnimationOnce(a_Name, a_Callback);
}

void cObject::PauseAnimation(bool a_Pause)
{
   m_AniManager.PauseAnimation(a_Pause);
}

void cObject::StepAnimation(uint32_t a_ElapsedMiliSec)
{
   //DebugTrace();
}


sf::Sprite* cObject::GetSprite()
{
   return m_AniManager.GetSprite();
}

bool cObject::IsVisible()
{
   return m_Visible;
}

void cObject::SetVisible(bool a_Visible)
{
   m_Visible = a_Visible;
}

///////////////////////////////////////////////////////////////////////
//
// Sound
//
///////////////////////////////////////////////////////////////////////

void cObject::PlaySound(std::string a_FileName)
{
   std::shared_ptr<cSound> l_Sound = GetResources()->LoadSoundBuffer(a_FileName);
   l_Sound->Play();
}

///////////////////////////////////////////////////////////////////////
//
// Collision Detection
//
//////////////////////////////////////////////////////////////////////

std::list<cObject*> cObject::GetCollisions(
   sf::Vector3<double> a_Position
   )
{
   std::list<cObject*> l_ReturnList;
   if (!m_Collidable)
   {
      return l_ReturnList;
   }

   if (GetType() == "Player1")
   {
      std::cout << "Getting Collisions" << std::endl;
   }

   std::list<cObject*> l_InterestingObjectList = GetBroadPhaseCollisions(a_Position);
   if (l_InterestingObjectList.size() ==0)
   {
      return l_ReturnList;
   }

   // Cache some values for the separating axis function------------------------
   //
   // Get the object's verticies. Inflate the box by 1 here because we don't
   // want objects within 1 of each other.
   std::vector<sf::Vector2<double>> l_Verts = GetInflatedVerticies(.999);

   // Move the rectangle to the new position
   sf::Vector3<double> a_RelativePosition(a_Position - GetPosition());
   for (auto i = l_Verts.begin(); i != l_Verts.end(); ++i)
   {
      i->x += a_RelativePosition.x;
      i->y += a_RelativePosition.y;
   }

   // Get the potential separating axis. This are just the normals to the faces
   // of the rectangle.
   std::vector<sf::Vector2<double>> l_Axis1;

   // First loop over each vertex and subtract it with the next one so that we
   // create edges with one point shifted to the origin. Then modify the edge to
   // make it perpendicular, which is just (x, y) => (-y, x) or (y, -x)
   for (auto i = l_Verts.begin(); i != l_Verts.end(); ++i)
   {
      sf::Vector2<double> l_Edge =
         (*i) - (((i + 1) == l_Verts.end()) ? *(l_Verts.begin()) : *(i + 1));

      sf::Vector2<double> l_Perp;
      l_Perp.x = -1 * l_Edge.y;
      l_Perp.y = l_Edge.x;

      // Normalize
      l_Perp = l_Perp / sqrt(pow(l_Perp.x,2)+pow(l_Perp.y,2));
      l_Axis1.push_back(l_Perp);
   }

   for (
         auto i = l_InterestingObjectList.begin();
         i != l_InterestingObjectList.end();
         ++i
       )
   {
      if (  *i == this
         || (*i)->Disabled()
         )
      {
         continue;
      }

      // If both objects are axis aligned, then just do the simple collision
      // algorithm
      if (  (*i)->GetRotation() % 90 == 0
         && GetRotation() % 90 == 0
         )
      {
         std::vector<sf::Vector2<double>> l_OtherVerts
            = (*i)->GetBoundingVerticies();
         if (GetCollisionsAxisAligned(l_Verts,l_OtherVerts))
         {
            l_ReturnList.push_back(*i);
         }
      }
      else
      {
         // Use the separating axis algorithm.
         //
         if (GetCollisionsSepAxis(l_Verts, l_Axis1, *i))
         {
            l_ReturnList.push_back(*i);
         }
      }
   }

   return l_ReturnList;
}

std::list<cObject*> cObject::GetBroadPhaseCollisions(
   sf::Vector3<double> a_Position
   )
{
   std::list<cObject*> l_ReturnList;

   sf::Vector3<double> l_NewRelativePosition
      = a_Position - GetPosition();

   sf::Rect<double> l_AABB = GetAxisAlignedBoundingRect();
   l_AABB.left += l_NewRelativePosition.x;
   l_AABB.top += l_NewRelativePosition.y;

   // Get first cell that a_pObject overlaps at new position
   std::pair<int32_t, int32_t> l_Cell;

   // Find how far to go in the x directin
   sf::Vector2<uint32_t> * l_pGridCellSize =
      GetResources()->GetActiveLevelData()->GetGridCellSize();

   std::map<std::pair<int32_t, int32_t>, std::list<cObject*>> * l_pCollisionMap =
      GetResources()->GetActiveLevelData()->GetCollisionMap();

   int32_t l_HorizontalCells =
      (l_AABB.left + l_AABB.width - .001) / l_pGridCellSize->x;
   int32_t l_VerticalCells =
      (l_AABB.top + l_AABB.height - .001) / l_pGridCellSize->y;

   for (l_Cell.first = l_AABB.left / l_pGridCellSize->x; l_Cell.first <= l_HorizontalCells; ++l_Cell.first)
   {
      for (l_Cell.second = l_AABB.top / l_pGridCellSize->y; l_Cell.second <= l_VerticalCells; ++l_Cell.second)
      {
         auto l_List
            = l_pCollisionMap->find(l_Cell);

         if (l_List != l_pCollisionMap->end())
         {
            l_ReturnList.insert(l_ReturnList.end(), (l_List->second).begin(), (l_List->second).end());
            //std::cout << "Adding: " << (l_List->second).size() << std::endl;
         }
      }
   }

   l_ReturnList.sort();
   l_ReturnList.unique();

   //std::cout << "Size:" << l_ReturnList.size() << std::endl;

   return l_ReturnList;
}

bool cObject::IsCollision(
   cObject* a_pObj2
   )
{
   if (  this == a_pObj2
      || Disabled()
      || a_pObj2->Disabled()
      )
   {
      return false;
   }

   std::list<cObject*> l_InterestingObjectList = GetBroadPhaseCollisions(GetPosition());

   std::list<cObject*>::iterator l_ObjectIt;
   for (
      l_ObjectIt = l_InterestingObjectList.begin();
      l_ObjectIt != l_InterestingObjectList.end();
      ++l_ObjectIt
      )
   {
      if (*l_ObjectIt == a_pObj2)
      {
         break;
      }
   }

   if (*l_ObjectIt != a_pObj2)
   {
      return false;
   }

   std::vector<sf::Vector2<double>> l_Verts = GetInflatedVerticies(.999);


   // If both objects are axis aligned, then just do the simple collision
   // algorithm
   if (  GetRotation() % 90 == 0
      && a_pObj2->GetRotation() % 90 == 0
      )
   {
      std::vector<sf::Vector2<double>> l_OtherVerts =
         a_pObj2->GetBoundingVerticies();
      if (GetCollisionsAxisAligned(l_Verts, l_OtherVerts))
      {
         return true;
      }
   }
   else
   {
      // Use the separating axis algorithm.


      // Get the potential separating axis. This are just the normals to the faces
      // of the rectangle.
      std::vector<sf::Vector2<double>> l_Axis1;

      // First loop over each vertex and subtract it with the next one so that we
      // create edges with one point shifted to the origin. Then modify the edge to
      // make it perpendicular, which is just (x, y) => (-y, x) or (y, -x)
      for (auto i = l_Verts.begin(); i != l_Verts.end(); ++i)
      {
         sf::Vector2<double> l_Edge =
            (*i) - (((i + 1) == l_Verts.end()) ? *(l_Verts.begin()) : *(i + 1));

         sf::Vector2<double> l_Perp;
         l_Perp.x = -1 * l_Edge.y;
         l_Perp.y = l_Edge.x;

         // Normalize
         l_Perp = l_Perp / sqrt(pow(l_Perp.x,2)+pow(l_Perp.y,2));
         l_Axis1.push_back(l_Perp);
      }

      if (GetCollisionsSepAxis(l_Verts, l_Axis1, a_pObj2))
      {
         return true;
      }
   }

   return false;
}

bool cObject::GetCollisionsSepAxis(
   std::vector<sf::Vector2<double>> a_ObjectRect,
   std::vector<sf::Vector2<double>> a_ObjectAxis,
   cObject * a_pOther
   )
{
   bool l_Overlap = true;

   std::vector<sf::Vector2<double>> l_Rect2 = a_pOther->GetBoundingVerticies();

   std::vector<sf::Vector2<double>> l_Axis2;
   for (auto j = l_Rect2.begin(); j != l_Rect2.end(); ++j)
   {
      sf::Vector2<double> l_Edge =
         (*j) - (((j + 1) == l_Rect2.end()) ? *(l_Rect2.begin()) : *(j + 1));

      sf::Vector2<double> l_Perp;
      l_Perp.x = -1 * l_Edge.y;
      l_Perp.y = l_Edge.x;
      l_Perp = l_Perp / sqrt(pow(l_Perp.x,2)+pow(l_Perp.y,2));

      // Normalize
      l_Axis2.push_back(l_Perp);
   }

   // For Axis List 1, project each shape to each axis. Save min and max
   // of each shape. This forms a line for each. See if the lines overlap.
   for (auto j = a_ObjectAxis.begin(); j != a_ObjectAxis.end(); ++j)
   {
      // Do rectangle 1 first
      double l_Min1;
      double l_Max1;
      for (auto l_Vert = a_ObjectRect.begin(); l_Vert != a_ObjectRect.end(); ++l_Vert)
      {
         // Projection with dot product
         double l_Proj = j->x * l_Vert->x + j->y * l_Vert->y;

         if (l_Vert == a_ObjectRect.begin())
         {
            l_Min1 = l_Proj;
            l_Max1 = l_Proj;
         }
         else if (l_Proj < l_Min1)
         {
            l_Min1 = l_Proj;
         }
         else if (l_Proj > l_Max1)
         {
            l_Max1 = l_Proj;
         }
      }
      // Now Do rectangle 2
      double l_Min2;
      double l_Max2;
      for (auto l_Vert = l_Rect2.begin(); l_Vert != l_Rect2.end(); ++l_Vert)
      {
         // Projection with dot product
         double l_Proj = j->x * l_Vert->x + j->y * l_Vert->y;

         if (l_Vert == l_Rect2.begin())
         {
            l_Min2 = l_Proj;
            l_Max2 = l_Proj;
         }
         else if (l_Proj < l_Min2)
         {
            l_Min2 = l_Proj;
         }
         else if (l_Proj > l_Max2)
         {
            l_Max2 = l_Proj;
         }
      }

      // Check for overlap. If at any point we don't get get an overlap we can
      // completly skip this object.
      if (l_Max1 < l_Min2 || l_Max2 < l_Min1)
      {
         l_Overlap = false;
         break;
      }
   }

   // Do it again for Axis list 2 unless we already determined there is no overlap
   if (l_Overlap == false)
   {
      return false;
   }


   for (auto j = l_Axis2.begin(); j != l_Axis2.end(); ++j)
   {
      // Do rectangle 1 first
      double l_Min1;
      double l_Max1;
      for (auto l_Vert = a_ObjectRect.begin(); l_Vert != a_ObjectRect.end(); ++l_Vert)
      {
         // Projection with dot product
         double l_Proj = j->x * l_Vert->x + j->y * l_Vert->y;

         if (l_Vert == a_ObjectRect.begin())
         {
            l_Min1 = l_Proj;
            l_Max1 = l_Proj;
         }
         else if (l_Proj < l_Min1)
         {
            l_Min1 = l_Proj;
         }
         else if (l_Proj > l_Max1)
         {
            l_Max1 = l_Proj;
         }
      }
      // Now Do rectangle 2
      double l_Min2;
      double l_Max2;
      for (auto l_Vert = l_Rect2.begin(); l_Vert != l_Rect2.end(); ++l_Vert)
      {
         // Projection with dot product
         double l_Proj = j->x * l_Vert->x + j->y * l_Vert->y;

         if (l_Vert == l_Rect2.begin())
         {
            l_Min2 = l_Proj;
            l_Max2 = l_Proj;
         }
         else if (l_Proj < l_Min2)
         {
            l_Min2 = l_Proj;
         }
         else if (l_Proj > l_Max2)
         {
            l_Max2 = l_Proj;
         }
      }

      // Check for overlap. If at any point we don't get get an overlap we can
      // completly skip this object.
      if (l_Max1 < l_Min2 || l_Max2 < l_Min1)
      {
         l_Overlap = false;
         break;
      }
   }

   // If we don't have an overlap then move on to the next object
   if (l_Overlap == false)
   {
      return false;
   }

   // At this point the objects still might not intersect. Check the z.

   double l_Peak1 = GetPosition().z + m_ZSize;
   double l_Peak2 = a_pOther->GetPosition().z + a_pOther->m_ZSize;

   if (  GetPosition().z >= a_pOther->GetPosition().z
      && GetPosition().z <= l_Peak2
      )
   {
      return true;
   }
   else if (  a_pOther->GetPosition().z >= GetPosition().z
           && a_pOther->GetPosition().z <= l_Peak1
           )
   {
      return true;
   }

   return l_Overlap;
}

bool cObject::GetCollisionsAxisAligned(
   std::vector<sf::Vector2<double>> a_ObjectBox,
   std::vector<sf::Vector2<double>> a_OtherBox
   )
{
   sf::Rect<double> l_ObjectBox;
   l_ObjectBox.left = a_ObjectBox[0].x;
   l_ObjectBox.top = a_ObjectBox[0].y;
   for (const auto& i : a_ObjectBox)
   {
      if (i.x < l_ObjectBox.left)
      {
         l_ObjectBox.left = i.x;
      }
      if (i.y < l_ObjectBox.top)
      {
         l_ObjectBox.top = i.y;
      }
      if (i.x - l_ObjectBox.left + 1 > l_ObjectBox.width)
      {
         l_ObjectBox.width = i.x - l_ObjectBox.left + 1;
      }
      if (i.y - l_ObjectBox.top + 1 > l_ObjectBox.height)
      {
         l_ObjectBox.height = i.y - l_ObjectBox.top + 1;
      }
   }


   sf::Rect<double> l_OtherBox;
   l_OtherBox.left = a_OtherBox[0].x;
   l_OtherBox.top = a_OtherBox[0].y;
   for (const auto& i : a_OtherBox)
   {
      if (i.x < l_OtherBox.left)
      {
         l_OtherBox.left = i.x;
      }
      if (i.y < l_OtherBox.top)
      {
         l_OtherBox.top = i.y;
      }
      if (i.x - l_OtherBox.left + 1 > l_OtherBox.width)
      {
         l_OtherBox.width = i.x - l_OtherBox.left + 1;
      }
      if (i.y - l_OtherBox.top + 1 > l_OtherBox.height)
      {
         l_OtherBox.height = i.y - l_OtherBox.top + 1;
      }
   }

   if (  l_OtherBox.left > l_ObjectBox.left + l_ObjectBox.width - 1
      || l_OtherBox.left + l_OtherBox.width - 1  < l_ObjectBox.left
      || l_OtherBox.top > l_ObjectBox.top + l_ObjectBox.height - 1
      || l_OtherBox.top + l_OtherBox.height - 1 < l_ObjectBox.top
      )
      {
         return false;
      }

   return true;
}


///////////////////////////////////////////////////////////////////////
//
// Position
//
//////////////////////////////////////////////////////////////////////

int32_t cObject::GetDepth()
{
   //DebugTrace();
   return m_Depth;
}

void cObject::SetDepth(int32_t a_Depth, ePositionType a_PosType)
{
   //DebugTrace();
   if (a_PosType == kNormal)
   {
      m_Depth = a_Depth;
   }
   else if (a_PosType == kRelative)
   {
      m_Depth += a_Depth;
   }

   std::list<cObject*> * l_pObjectList =
      GetResources()->GetActiveLevelData()->GetObjectList();

   l_pObjectList->sort(cObject::ObjectOrder);
}

sf::Vector3<double> cObject::GetVelocity()
{
   return m_Velocity;
}

void cObject::SetVelocity(sf::Vector3<double> a_Velocity, ePositionType a_Type)
{
   if (a_Type == kNormal)
   {
      m_Velocity = a_Velocity;
   }
   else if (a_Type == kRelative)
   {
      m_Velocity += a_Velocity;
   }
   else
   {
      DebugPrintf("WARNING: Position Type is not implemented!\n");
   }

}
void cObject::SetVelocityX(double a_Velocity, ePositionType a_Type)
{
   if (a_Type == kNormal)
   {
      m_Velocity.x = a_Velocity;
   }
   else if (a_Type == kRelative)
   {
      m_Velocity.x += a_Velocity;
   }
   else
   {
      DebugPrintf("WARNING: Position Type is not implemented!\n");
   }
}
void cObject::SetVelocityY(double a_Velocity, ePositionType a_Type)
{
   if (a_Type == kNormal)
   {
      m_Velocity.y = a_Velocity;
   }
   else if (a_Type == kRelative)
   {
      m_Velocity.y += a_Velocity;
   }
   else
   {
      DebugPrintf("WARNING: Position Type is not implemented!\n");
   }
}
void cObject::SetVelocityZ(double a_Velocity, ePositionType a_Type)
{
   if (a_Type == kNormal)
   {
      m_Velocity.z = a_Velocity;
   }
   else if (a_Type == kRelative)
   {
      m_Velocity.z += a_Velocity;
   }
   else
   {
      DebugPrintf("WARNING: Position Type is not implemented!\n");
   }
}


sf::Vector3<double> cObject::GetPosition()
{
   return m_Position;
}

sf::Vector3<double> cObject::GetPreviousPosition()
{
   return m_PreviousPosition;
}

void cObject::SetPosition(
   sf::Vector3<double> a_Position,
   ePositionType a_PosType,
   bool a_CheckForCollisions,
   bool a_InterpolateMovement
   )
{
   if (m_PreviousPosition == GetPosition())
   {
      // Other objects haven't moved us yet, so this is safe.
      m_PreviousFramePosition = GetPosition();
   }

   sf::Vector3<double> l_NewPosition = m_Position;

   switch  (a_PosType)
   {
      case kNormal:
      {
         l_NewPosition = a_Position;
         break;
      }
      case kRelative:
      {
         l_NewPosition.x += a_Position.x;
         l_NewPosition.y += a_Position.y;
         l_NewPosition.z += a_Position.z;
         break;
      }
      default:
      {
         DebugPrintf("WARNING: Position Type is not implemented!\n");
      }
   }

   if (l_NewPosition != m_Position)
   {
      sf::Vector3<double> l_RelativePositionChange = l_NewPosition - m_Position;

      m_Position = l_NewPosition;

      if (a_CheckForCollisions)
      {
         std::list<cObject*> l_CollisionList = GetCollisions(m_Position);

         for (auto i = l_CollisionList.begin(); i != l_CollisionList.end(); ++i)
         {
            Collision(*i);
            (*i)->Collision(this);
         }
      }

      // Move all child objects
      for (cObject* l_pChildObject : m_ChildObjects)
      {
         l_pChildObject->SetPosition(
            l_RelativePositionChange,
            kRelative,
            a_CheckForCollisions,
            a_InterpolateMovement
            );
      }
   }

   if (!a_InterpolateMovement)
   {
      // Teleport to the position
      m_PreviousFramePosition = GetPosition();
   }

   m_AniManager.SetPosition(m_Position.x, m_Position.y);

}

int32_t cObject::GetRotation()
{
   // TODO: Implement rotation
   return 0;
}

sf::Rect<int32_t> cObject::GetBoundingBox()
{
   return m_AniManager.GetBoundingBox();
}

void cObject::SetBoundingBox(const sf::Rect<int32_t> & a_rkBoundingBox)
{
   m_AniManager.SetBoundingBox(a_rkBoundingBox);
}

void cObject::RemoveFromCollisionMap(bool a_UsePreviousPosition)
{
   if (!m_Collidable)
   {
      return;
   }

   sf::Vector3<double> l_NewRelativePosition(0,0,0);

   if (a_UsePreviousPosition)
   {
      l_NewRelativePosition = m_PreviousPosition - m_Position;
   }

   sf::Rect<double> l_AABB = GetAxisAlignedBoundingRect();
   l_AABB.left += l_NewRelativePosition.x;
   l_AABB.top += l_NewRelativePosition.y;

   // Get first cell that we used to overlap
   std::pair<int32_t, int32_t> l_Cell;

   sf::Vector2<uint32_t> * l_pGridCellSize =
      GetResources()->GetActiveLevelData()->GetGridCellSize();

   std::map<std::pair<int32_t, int32_t>, std::list<cObject*>> * l_pCollisionMap =
      GetResources()->GetActiveLevelData()->GetCollisionMap();

   // Find how far to go in the x directin
   int32_t l_HorizontalCells =
      (l_AABB.left + l_AABB.width - .001) / l_pGridCellSize->x;
   int32_t l_VerticalCells =
      (l_AABB.top + l_AABB.height - .001) / l_pGridCellSize->y;


   for (l_Cell.first = l_AABB.left / l_pGridCellSize->x; l_Cell.first <= l_HorizontalCells; ++l_Cell.first)
   {
      for (l_Cell.second = l_AABB.top / l_pGridCellSize->y; l_Cell.second <= l_VerticalCells; ++l_Cell.second)
      {
         (*(l_pCollisionMap))[l_Cell].remove(this);
      }
   }
}

void cObject::AddToCollisionMap()
{
   if (!m_Collidable)
   {
      return;
   }

   sf::Rect<double> l_AABB = GetAxisAlignedBoundingRect();

   sf::Vector2<uint32_t> * l_pGridCellSize =
      GetResources()->GetActiveLevelData()->GetGridCellSize();

   std::map<std::pair<int32_t, int32_t>, std::list<cObject*>> * l_pCollisionMap =
      GetResources()->GetActiveLevelData()->GetCollisionMap();

   // Get first cell that a_pObject overlaps at new position
   std::pair<int32_t, int32_t> l_Cell;

   // Find how far to go in the x directin
   int32_t l_HorizontalCells =
      (l_AABB.left + l_AABB.width - .001) / l_pGridCellSize->x;
   //std::cout << "H: " << l_HorizontalCells << "|";

   int32_t l_VerticalCells =
      (l_AABB.top + l_AABB.height - .001) / l_pGridCellSize->y;

   for (l_Cell.first = l_AABB.left / l_pGridCellSize->x; l_Cell.first <= l_HorizontalCells; ++l_Cell.first)
   {
      for (l_Cell.second = l_AABB.top / l_pGridCellSize->y; l_Cell.second <= l_VerticalCells; ++l_Cell.second)
      {
         //std::cout << "(" << l_Cell.first << "," << l_Cell.second << ")";
         (*(l_pCollisionMap))[l_Cell].push_back(this);

      }
   }
   //std::cout << std::endl;
}

std::vector<sf::Vector2<double>> cObject::GetBoundingVerticies()
{
   // TODO: Handle rotation
   sf::Rect<int32_t> l_Box = m_AniManager.GetBoundingBox();

   sf::Vector2<double> l_V1(GetPosition().x + l_Box.left, GetPosition().y + l_Box.top);

   // Subtract 1 to prevent off by 1 errors
   sf::Vector2<double> l_V2(
        GetPosition().x + l_Box.left + l_Box.width - 1,
        GetPosition().y + l_Box.top
        );

   sf::Vector2<double> l_V3(
      GetPosition().x + l_Box.left + l_Box.width - 1,
      GetPosition().y + l_Box.top + l_Box.height - 1
      );

   sf::Vector2<double> l_V4(
      GetPosition().x + l_Box.left,
      GetPosition().y + l_Box.top + l_Box.height - 1
      );

   std::vector<sf::Vector2<double>> l_Return;
   l_Return.push_back(l_V1);
   l_Return.push_back(l_V2);
   l_Return.push_back(l_V3);
   l_Return.push_back(l_V4);

   return l_Return;
}

std::vector<sf::Vector2<double>> cObject::GetInflatedVerticies(double a_Padding)
{
   // TODO: Handle rotation
   sf::Rect<int32_t> l_Box = m_AniManager.GetBoundingBox();

   // Add the padding
   double l_Left = l_Box.left - a_Padding;
   double l_Top = l_Box.top - a_Padding;
   double l_Width = l_Box.width + a_Padding * 2;
   double l_Height = l_Box.height + a_Padding * 2;

   sf::Vector2<double> l_V1(
      GetPosition().x + l_Left,
      GetPosition().y + l_Top
      );

   // Subtract 1 to prevent off by 1 errors. For example, if left is at 0 and
   // width is 32, the vertex should be at 31.
   sf::Vector2<double> l_V2(
        GetPosition().x + l_Left + l_Width - 1,
        GetPosition().y + l_Top
        );

   sf::Vector2<double> l_V3(
      GetPosition().x + l_Left + l_Width - 1,
      GetPosition().y + l_Top + l_Height - 1
      );

   sf::Vector2<double> l_V4(
      GetPosition().x + l_Left,
      GetPosition().y + l_Top + l_Height - 1
      );

   std::vector<sf::Vector2<double>> l_Return;
   l_Return.push_back(l_V1);
   l_Return.push_back(l_V2);
   l_Return.push_back(l_V3);
   l_Return.push_back(l_V4);

   return l_Return;
}


sf::Rect<double> cObject::GetAxisAlignedBoundingRect()
{
   std::vector<sf::Vector2<double>> l_Verts = GetBoundingVerticies();

   sf::Rect<double> l_ReturnValue(l_Verts[0].x, l_Verts[0].y, 0, 0);
   double l_RightMost = l_Verts[0].x;
   double l_BottomMost = l_Verts[0].y;

   for (auto l_Vert : l_Verts)
   {
      if (l_Vert.x < l_ReturnValue.left)
      {
         l_ReturnValue.left = l_Vert.x;
      }
      else if (l_Vert.x > l_RightMost)
      {
         l_RightMost = l_Vert.x;
      }

      if (l_Vert.y < l_ReturnValue.top)
      {
         l_ReturnValue.top = l_Vert.y;
      }
      else if (l_Vert.y > l_BottomMost)
      {
         l_BottomMost = l_Vert.y;
      }
   }

   l_ReturnValue.width = l_RightMost - l_ReturnValue.left + 1;
   l_ReturnValue.height = l_BottomMost - l_ReturnValue.top + 1;

   return l_ReturnValue;
}

std::list<cObject*> cObject::MoveFlush(
   sf::Vector3<double> a_Position,
   ePositionType a_PosType
   )
{
   if (m_PreviousPosition == GetPosition())
   {
      // Other objects haven't moved us yet, so this is safe.
      m_PreviousFramePosition = GetPosition();
   }

   if (a_PosType != kRelative)
   {
      std::list<cObject*> l_Temp;
      return l_Temp;
   }

   if (a_Position.x == 0 && a_Position.y == 0 && a_Position.z == 0)
   {
      std::list<cObject*> l_Temp;
      return l_Temp;
   }

   sf::Vector3<double> l_NewAbsolutePosition = a_Position + m_Position;

   // This will be the final position after taking collisions into account
   sf::Vector3<double> l_AdjustedPosition = a_Position;

   // This will be how much we need to actually shift to sit flush
   double l_XShift = INFINITY;
   double l_YShift = INFINITY;

   std::list<cObject*> l_CollisionList =
      GetCollisions(l_NewAbsolutePosition);

   // This list should be set to the last non-empty l_CollisionList so that we
   // Can alert those objects of a collision
   std::list<cObject*> l_SavedCollisionList = l_CollisionList;

   bool l_SkipX = false;
   bool l_SkipY = false;

   // Don't want to get stuck in an infinite loop so quit after a while
   uint32_t l_LoopCount = 0;
   while ((l_CollisionList.size() != 0) && (++l_LoopCount < 6))
   {
      //~ if (l_LoopCount == 3)
      //~ {
         //~ std::cout << "Potential problem in MoveFlush" << std::endl;
      //~ }
      //~ if (l_LoopCount == 5)
      //~ {
         //~ std::cout << "YUPPP" << std::endl;
      //~ }
      // Inflate this rectangle by 1 because we don't want other objects that
      // close.
      std::vector<sf::Vector2<double>> l_Me = GetInflatedVerticies(.999);

      bool l_ValidObjects = false;
      for (auto c = l_CollisionList.begin(); c != l_CollisionList.end(); ++c)
      {
         if ((*c)->IsSolid() == false)
         {
            continue;
         }

         l_ValidObjects = true;

         std::vector<sf::Vector2<double>> l_Other =
            (*c)->GetBoundingVerticies();

         for (auto i = l_Other.begin(); i != l_Other.end(); ++i)
         {
            sf::Vector2<double> i2 =
               ((i + 1) == l_Other.end()) ? *(l_Other.begin()) : *(i + 1);


            for (auto j = l_Me.begin(); j != l_Me.end(); ++j)
            {
               sf::Vector2<double> j2 =
               ((j + 1) == l_Me.end()) ? *(l_Me.begin()) : *(j + 1);

               // Test X first.
               if (!l_SkipX)
               {
                  //Multiply by -1 because we're actually wanting to move j, not i.
                  double l_Candidate = ShiftPointXToLine(*i, *j, j2) * -1;
                  if (fabs(l_Candidate) < fabs(l_XShift))
                  {
                     l_XShift = l_Candidate;
                  }

                  //Multiply by -1 because we're actually wanting to move j, not i.
                  l_Candidate = ShiftPointXToLine(i2, *j, j2) * -1;
                  if (fabs(l_Candidate) < fabs(l_XShift))
                  {
                     l_XShift = l_Candidate;
                  }

                  l_Candidate = ShiftPointXToLine(*j, *i, i2);
                  if (fabs(l_Candidate) < fabs(l_XShift))
                  {
                     l_XShift = l_Candidate;
                  }

                  l_Candidate = ShiftPointXToLine(j2, *i, i2);
                  if (fabs(l_Candidate) < fabs(l_XShift))
                  {
                     l_XShift = l_Candidate;
                  }
               }

               // Test Y
               if (!l_SkipY)
               {
                  //Multiply by -1 because we're actually wanting to move j, not i.
                  double l_Candidate = ShiftPointYToLine(*i, *j, j2) * -1;
                  if (fabs(l_Candidate) < fabs(l_YShift))
                  {
                     l_YShift = l_Candidate;
                  }

                  //Multiply by -1 because we're actually wanting to move j, not i.
                  l_Candidate = ShiftPointYToLine(i2, *j, j2) * -1;
                  if (fabs(l_Candidate) < fabs(l_YShift))
                  {
                     l_YShift = l_Candidate;
                  }

                  l_Candidate = ShiftPointYToLine(*j, *i, i2);
                  if (fabs(l_Candidate) < fabs(l_YShift))
                  {
                     l_YShift = l_Candidate;
                  }

                  l_Candidate = ShiftPointYToLine(j2, *i, i2);
                  if (fabs(l_Candidate) < fabs(l_YShift))
                  {
                     l_YShift = l_Candidate;
                  }
               }
            }
         }
      }

      if (l_ValidObjects == false)
      {
         // Every object is non-solid
         break;
      }

      // Move back a bit so that we're not still colliding.
      double l_XShiftAdjusted = l_XShift;
      double l_YShiftAdjusted = l_YShift;
      double l_Modifier = 0.001;

      if (l_SkipX)
      {
         l_XShiftAdjusted = 0;
      }
      else if (l_XShiftAdjusted == INFINITY)
      {
         l_XShiftAdjusted = a_Position.x;
      }
      else if (l_XShiftAdjusted > 0)
      {
         l_XShiftAdjusted -= l_Modifier;
      }
      else if (l_XShiftAdjusted < 0)
      {
         l_XShiftAdjusted += l_Modifier;
      }

      if (l_SkipY)
      {
         l_YShiftAdjusted = 0;
      }
      else if (l_YShiftAdjusted == INFINITY)
      {
         l_YShiftAdjusted = a_Position.y;
      }
      else if (l_YShiftAdjusted > 0)
      {
         l_YShiftAdjusted -= l_Modifier;
      }
      else if (l_YShiftAdjusted < 0)
      {
         l_YShiftAdjusted += l_Modifier;
      }

      l_NewAbsolutePosition.x = l_XShiftAdjusted + m_Position.x;
      l_NewAbsolutePosition.y = l_YShiftAdjusted + m_Position.y;
      l_AdjustedPosition.x = l_XShiftAdjusted;
      l_AdjustedPosition.y = l_YShiftAdjusted;

      // Double check that there aren't more collisions
      l_CollisionList = GetCollisions(l_NewAbsolutePosition);
      if (l_CollisionList.size() != 0)
      {
         l_SavedCollisionList = l_CollisionList;

         // There is a case where xshift and yshift are the same as a_Position
         // even though applying both would cause a collision (moving diagonally
         // and hitting a corner). In that case, only move the larger of the 2
         // and start the loop again. Not using the SetPosition function because
         // we don't want to worry about sprites or messaging
         if (l_XShiftAdjusted == a_Position.x && l_YShiftAdjusted == a_Position.y)
         {
            if (fabs(l_AdjustedPosition.x) > fabs(l_AdjustedPosition.y))
            {
               // Make sure this move doesn't cause another collision
               sf::Vector3<double> l_TempPosition = m_Position;
               l_TempPosition.x += l_AdjustedPosition.x;
               std::list<cObject*> l_TempList =
                  GetCollisions(l_TempPosition);

               if (l_TempList.size() == 0)
               {
                  m_Position.x += l_AdjustedPosition.x;
               }

               l_SkipX = true;
            }
            else
            {
               // Make sure this move doesn't cause another collision
               sf::Vector3<double> l_TempPosition = m_Position;
               l_TempPosition.y += l_AdjustedPosition.y;
               std::list<cObject*> l_TempList =
                  GetCollisions(l_TempPosition);

               if (l_TempList.size() == 0)
               {
                  m_Position.y += l_AdjustedPosition.y;
               }

               l_SkipY = true;
            }
         }
      }
   }

   SetPosition(l_AdjustedPosition, kRelative, false);

   // Alert objects who were actually involved with the final collision.
   return l_SavedCollisionList;
}

bool cObject::SitFlush(cObject* a_pOtherObject)
{
   m_Position = m_PreviousPosition;
   std::list<cObject*> l_CollisionList =
      MoveFlush(m_AdjustedVelocity, kRelative);

   for (auto i = l_CollisionList.begin(); i != l_CollisionList.end(); ++i)
   {
      if (*i == a_pOtherObject)
      {
         return true;
      }
   }
   return false;
}

void cObject::AddChild(cObject* a_pChild)
{
   if (a_pChild == NULL)
   {
      return;
   }

   // Make sure the object doesn't already exist
   for (cObject* l_pChild : m_ChildObjects)
   {
      if (l_pChild->GetUniqueId() == a_pChild->GetUniqueId())
      {
         return;
      }
   }

   m_ChildObjects.push_back(a_pChild);
   a_pChild->SetParent(this);
}

// Remove child
void cObject::RemoveChild(cObject* a_pChild)
{
   if (a_pChild == NULL)
   {
      return;
   }

   for (std::vector<cObject*>::iterator i = m_ChildObjects.begin();
      i != m_ChildObjects.end();
      ++i
      )
   {
      if ((*i)->GetUniqueId() == a_pChild->GetUniqueId())
      {
         m_ChildObjects.erase(i);
         if (a_pChild->m_pParentObject->GetUniqueId() == GetUniqueId())
         {
            a_pChild->SetParent(NULL);
         }
         return;
      }
   }
}

void cObject::RemoveAllChildren()
{
   for (cObject* l_pChild : m_ChildObjects)
   {
      if (l_pChild->m_pParentObject->GetUniqueId() == GetUniqueId())
      {
         l_pChild->m_pParentObject = NULL;
      }
   }

   m_ChildObjects.clear();
}

// Add Parent
void cObject::SetParent(cObject* a_pParent)
{
   if (m_pParentObject != NULL)
   {
      m_pParentObject->RemoveChild(this);
   }
   m_pParentObject = a_pParent;
}

