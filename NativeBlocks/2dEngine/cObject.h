#ifndef ___cObject_h___
#define ___cObject_h___

#include "Common.h"
#include "cAniManager.h"
#include "cResources.h"
#include "cView.h"

// SFML
#include "SFML/Graphics.hpp"

#include <list>

enum ePositionType
{
   kNormal,   // Set position relative to game world
   kRelative, // Change relative to current object value
   kAbsolute  // Change relative to Window Coordinates (dispite camera)
};

class cObject
{
public:
   cObject(cResources* a_pResources);
   virtual ~cObject();

   // These functions are called by the Level
   virtual void Initialize();
   void StartStep (uint32_t a_ElapsedMiliSec);
   virtual void Step (uint32_t a_ElapsedMiliSec);
   void StartDraw(double a_InterpolateFactor);
   virtual void Draw (const sf::Vector2<float> & a_rkOffset);
   void StartEvent(std::list<sf::Event> * a_pEventList);
   virtual void Event(std::list<sf::Event> * a_pEventList);
   virtual void Collision(cObject* a_pOther);

   ///////////////////////////////////////////////////////////////////////
   //
   // General Properties
   //
   //////////////////////////////////////////////////////////////////////

   int32_t GetUniqueId();
   bool IsSolid();
   void SetSolid(bool a_IsSolid);
   bool IsCollidable();
   void SetCollidable(bool a_IsCollidable);
   void SetType(std::string a_Type);
   std::string GetType(void);

   // Disabled objects will skip drawing, stepping, etc.
   void Disable(bool a_Disable);
   bool Disabled();

   bool WaitingToBeDestroyed();

   // Persistent objects will still exist on a level change
   void Persistent(bool a_Persistent);
   bool Persistent();

   void UnregisterObject(bool a_Delete);

   // used by the list sorter
   static bool ObjectOrder(cObject* a_ObjectA, cObject* a_ObjectB);

   void SetView(cView* a_pView);

   cResources* GetResources();

   ///////////////////////////////////////////////////////////////////////
   //
   // Graphics
   //
   //////////////////////////////////////////////////////////////////////

   // Load the Sprites and animations from an animation descriptor text file.
   bool LoadAnimations(std::string a_FileName);

   // Set the animation to something that loops (such as running). This can
   // double as a static sprite if the animation only has 1 frame.
   void PlayAnimationLoop(std::string a_Name);

   // Play an animation that does not loop (such as jumping). When this
   // animation is done the previous static sprite or looping animation should
   // start again.
   void PlayAnimationOnce(
      std::string a_Name,
      std::function<void(void)> a_Callback = NULL
      );

   void PauseAnimation(bool a_Pause);

   void StepAnimation(uint32_t a_ElapsedMiliSec);

   // Gets the current animation frame
   sf::Sprite* GetSprite();

   bool IsVisible();
   void SetVisible(bool a_Visible);

   virtual int32_t GetDepth();
   void SetDepth(int32_t a_Depth, ePositionType a_PosType = kNormal);

   ///////////////////////////////////////////////////////////////////////
   //
   // Sound
   //
   ///////////////////////////////////////////////////////////////////////

   void PlaySound(std::string a_FileName);

   ///////////////////////////////////////////////////////////////////////
   //
   // Collision Detection
   //
   //////////////////////////////////////////////////////////////////////

   // This function is used by objects to determine if there will be a collision
   // during movement. Usually the a_pObject variable will be a this pointer.
   // The return value is a list of objects it collided with.
   std::list<cObject*> GetCollisions(
      sf::Vector3<double> a_Position
      );

   // This function narrows down the list of objects we should check for
   // collisions
   std::list<cObject*> GetBroadPhaseCollisions(
      sf::Vector3<double> a_Position
      );

   // This function is used to see if 2 objects are already colliding
   bool IsCollision(cObject* a_pObj2);

   // This function can handle rotated objects.
   bool GetCollisionsSepAxis(
      std::vector<sf::Vector2<double>> a_ObjectRect,
      std::vector<sf::Vector2<double>> a_ObjectAxis,
      cObject* a_pOther
      );

   // This function is only valid for axis aligned objects.
   bool GetCollisionsAxisAligned(
      std::vector<sf::Vector2<double>> a_ObjectBox,
      std::vector<sf::Vector2<double>> a_OtherBox
      );

   // Objects must be in the collision map to collide.
   void AddToCollisionMap();
   void RemoveFromCollisionMap(bool a_UsePreviousPosition);

   sf::Rect<int32_t> GetBoundingBox();
   void SetBoundingBox(const sf::Rect<int32_t> & a_rkBoundingBox);


   // Return a list of the Bounding Box verticies in absolute coordinates.
   std::vector<sf::Vector2<double>> GetBoundingVerticies();

   // This is more of a helper. Useful when we don't want objects within 1 of
   // each other
   std::vector<sf::Vector2<double>> GetInflatedVerticies(double a_Padding);

   // This function draws a rectangle around the verticies. Useful for
   // broadphase collision detection
   sf::Rect<double> GetAxisAlignedBoundingRect();

   ///////////////////////////////////////////////////////////////////////
   //
   // Position
   //
   //////////////////////////////////////////////////////////////////////

   sf::Vector3<double> GetVelocity();
   void SetVelocity(sf::Vector3<double> a_Velocity, ePositionType a_Type);
   void SetVelocityX(double a_Velocity, ePositionType a_Type);
   void SetVelocityY(double a_Velocity, ePositionType a_Type);
   void SetVelocityZ(double a_Velocity, ePositionType a_Type);
   sf::Vector3<double> GetPosition();
   sf::Vector3<double> GetPreviousPosition();
   void SetPosition(
      sf::Vector3<double> a_Position,
      ePositionType a_PosType = kNormal,
      bool a_CheckForCollisions = true,
      bool a_InterpolateMovement = true
      );
   int32_t GetRotation();

   // Translate a solid object until it runs into another solid object or reaches
   // its destination. This function is not intended for moving farther than
   // an object size per step. Returns a list of objects it is flush with.
    std::list<cObject*> MoveFlush(
      sf::Vector3<double> a_Position,
      ePositionType a_PosType = kNormal
      );

   // During collision use this function to move flush with object. Returns true
   // if there was a collision when moving flush. This lets you know that the
   // collision was just a result of tunneling. TODO: What does this comment
   // mean again?
   bool SitFlush(cObject* a_pOtherObject);

   // This variable is only used to store depth size. The sprite's rectangle
   // contains the size of the other 2 corrdinates. This variable should be 0
   // Unless we're doing 3d type stuff.
   double m_ZSize;

   // Add child
   void AddChild(cObject* a_pChild);

   // Remove child
   void RemoveChild(cObject* a_pChild);

   void RemoveAllChildren();

   // Add Parent
   void SetParent(cObject* a_pParent);

private:
   cResources* m_pResources;

   cView* m_pView;
   int32_t m_UniqueId;

   ///////////////////////////////////////////////////////////////////////
   //
   // General Properties
   //
   //////////////////////////////////////////////////////////////////////
   bool m_Solid;
   bool m_Collidable;
   std::string m_Type;
   bool m_Disabled;
   bool m_ToBeDestroyed;

   // Persistent between level switching
   bool m_Persistent;

   ///////////////////////////////////////////////////////////////////////
   //
   // Graphics
   //
   //////////////////////////////////////////////////////////////////////
   cAniManager m_AniManager;
   bool m_Visible;

   // Used for interpolation. Can't just use m_PreviousPosition because that one
   // gets set to the real position before Draw(), therefore preventing
   // interpolation from working.
   sf::Vector3<double> m_PreviousFramePosition;

   ///////////////////////////////////////////////////////////////////////
   //
   // Position
   //
   //////////////////////////////////////////////////////////////////////

   // z is depth
   sf::Vector3<double> m_Position;

   // Gets updated during movement. Useful for moving back if we had a collision
   // due to velocity
   sf::Vector3<double> m_PreviousPosition;

   // Speed is in pixels/second
   sf::Vector3<double> m_Velocity;

   // This gets updated each step to take into account frame rate
   sf::Vector3<double> m_AdjustedVelocity;

   // This variable helps determine drawing order
   int32_t m_Depth;

   std::vector<cObject*> m_ChildObjects;
   cObject* m_pParentObject;

};

#endif
