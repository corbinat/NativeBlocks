///////////////////////////////////////////////////////////////////////
//
// Includes
//
//////////////////////////////////////////////////////////////////////

#include "cAniManager.h"
#include "cVFileNode.h"
#include "cResources.h"
#include "Common.h"

#include <sstream>
#include <iostream>

///////////////////////////////////////////////////////////////////////
//
// Class Definition
//
//////////////////////////////////////////////////////////////////////
cAniManager::cAniManager(cResources* a_pResources)
   : m_pResources(a_pResources),
     m_AnimationMap(),
     m_CurrentAnimation(),
     m_LastAnimation(),
     m_BoundingBox(),
     m_AnimationEnded(false),
     m_PlayingAnimationOnce(false),
     m_Callback(NULL),
     m_Paused(false)
{
}

cAniManager::~cAniManager()
{
}

bool cAniManager::LoadAnimations(std::string a_FileName)
{
   if (m_pResources == NULL)
   {
      DebugPrintf("ERROR: Manager is NULL\n");
      return false;
   }

   // See if we have this animation file in cache
   // TODO: This only allows having animations from one file. Do I need to
   // support multiple files?
   m_AnimationMap = m_pResources->GetCachedAnimationMap(a_FileName);

   if (m_AnimationMap.size() != 0)
   {
      return true;
   }

   // Not in cache. Actually load it from file.
   std::shared_ptr<cVFileNode> l_pFile(
      m_pResources->LoadcVFile(a_FileName)
      );

   if (!l_pFile)
   {
      DebugPrintf("ERROR: Failed to load animation file\n");
      return false;
   }

   size_t l_AnimationCount = (*l_pFile)["Animations"].Size();
   for (size_t i = 0; i < l_AnimationCount; ++i)
   {
      bool l_Success;

      std::string l_Name = (*l_pFile)["Animations"][i]["Name"];

      m_AnimationMap[l_Name] = cAnimation();

      std::istringstream l_SpeedStream(
         (*l_pFile)["Animations"][i]["Speed"]
         );
      uint32_t l_Speed;
      l_SpeedStream >> l_Speed;
      m_AnimationMap[l_Name].SetSpeed(l_Speed);

      // Add the frames to the animation
      size_t l_FrameCount = (*l_pFile)["Animations"][i]["Frames"].Size();
      for (size_t j = 0; j < l_FrameCount; ++j)
      {
         // Get the sprite's texture
         std::string l_Texture =
            (*l_pFile)["Animations"][i]["Frames"][j]["Image"];

         // Get the sprite's size
         std::istringstream l_WidthStream(
            (*l_pFile)["Animations"][i]["Frames"][j]["Width"]
            );
         uint32_t l_Width;
         l_WidthStream >> l_Width;

         std::istringstream l_HeightStream(
            (*l_pFile)["Animations"][i]["Frames"][j]["Height"]
            );
         uint32_t l_Height;
         l_HeightStream >> l_Height;

         // Get the sprite's Position on the texture
         std::istringstream l_XStream(
            (*l_pFile)["Animations"][i]["Frames"][j]["X"]
            );
         uint32_t l_X;
         l_XStream >> l_X;

         std::istringstream l_YStream(
            (*l_pFile)["Animations"][i]["Frames"][j]["Y"]
            );
         uint32_t l_Y;
         l_YStream >> l_Y;

         // Get the Bounding Box's Size
         // TODO: Support more than 1 bounding box.
         std::istringstream l_BWidthStream(
            (*l_pFile)["Animations"][i]["Frames"][j]["BoundingBoxes"][0]["Width"]
            );
         int32_t l_BWidth;
         l_BWidthStream >> l_BWidth;

         std::istringstream l_BHeightStream(
            (*l_pFile)["Animations"][i]["Frames"][j]["BoundingBoxes"][0]["Height"]
            );
         int32_t l_BHeight;
         l_BHeightStream >> l_BHeight;

         // Get the Bounding Box's Position
         std::istringstream l_BXStream(
            (*l_pFile)["Animations"][i]["Frames"][j]["BoundingBoxes"][0]["X"]
            );
         int32_t l_BX;
         l_BXStream >> l_BX;

         std::istringstream l_BYStream(
            (*l_pFile)["Animations"][i]["Frames"][j]["BoundingBoxes"][0]["Y"]
            );
         int32_t l_BY;
         l_BYStream >> l_BY;


         l_Success =
            m_AnimationMap[l_Name].AddFrame(
               m_pResources->LoadTexture(l_Texture),
               l_Height,
               l_Width,
               l_X,
               l_Y,
               sf::Rect<int32_t>(l_BX,l_BY,l_BWidth,l_BHeight)
               );

         if (l_Success == false)
         {
            DebugPrintf("Failed to load animation\n");
            break;
         }
      }

      if (l_Success == false)
      {
         // Try to continue on and load other animations
         continue;
      }

      // Set the default current animation to the first one we find.
      if (m_AnimationMap.size() == 1)
      {
         m_CurrentAnimation = l_Name;
      }
   }

   m_pResources->CacheAnimationMap(a_FileName, m_AnimationMap);

   return true;
}

// Set the animation to something that loops (such as running). This can
// double as a static sprite if the animation only has 1 frame.
void cAniManager::PlayAnimationLoop(std::string a_Name)
{
   m_PlayingAnimationOnce = false;
   m_CurrentAnimation = a_Name;
   m_AnimationMap[m_CurrentAnimation].SetFrame(0);
}

// start again.
void cAniManager::PlayAnimationOnce(
   std::string a_Name,
   std::function<void(void)> a_Callback
   )
{
   m_PlayingAnimationOnce = true;
   m_LastAnimation = m_CurrentAnimation;
   m_CurrentAnimation = a_Name;
   if (!m_CurrentAnimation.empty())
   {
      m_AnimationMap[m_CurrentAnimation].SetFrame(0);
   }
   m_Callback = a_Callback;
}

void cAniManager::PauseAnimation(bool a_Pause)
{
   m_Paused = a_Pause;
}

void cAniManager::StepAnimation(uint32_t a_ElapsedMiliSec)
{
   if (!m_CurrentAnimation.empty() && !m_Paused)
   {
      m_AnimationMap[m_CurrentAnimation].Step(a_ElapsedMiliSec);

      if (m_AnimationMap[m_CurrentAnimation].AnimationHasEnded())
      {
         if (m_PlayingAnimationOnce)
         {
            m_CurrentAnimation = m_LastAnimation;
            m_LastAnimation = "";
            m_PlayingAnimationOnce = false;

            if (m_Callback != NULL)
            {
               m_Callback();
            }
         }
         else
         {
            m_AnimationMap[m_CurrentAnimation].SetFrame(0);
         }
      }
   }
}

void cAniManager::SetPosition(double a_X, double a_Y)
{
   // For every animation set the position of every sprite.
   for (auto& l_rAnimation : m_AnimationMap)
   {
      l_rAnimation.second.SetPosition(a_X, a_Y);
   }
}

// Gets the current animation frame
sf::Sprite* cAniManager::GetSprite()
{
   if (!m_CurrentAnimation.empty())
   {
      return m_AnimationMap[m_CurrentAnimation].GetCurrentSprite();
   }
   else
   {
      return NULL;
   }
}

// Gets the current frame's bounding box
sf::Rect<int32_t> cAniManager::GetBoundingBox()
{
   if (!m_CurrentAnimation.empty())
   {
      return m_AnimationMap[m_CurrentAnimation].GetBoundingBox();
   }
   else
   {
      return m_BoundingBox;
   }
}

void cAniManager::SetBoundingBox(sf::Rect<int32_t> a_BoundingBox)
{
   m_BoundingBox = a_BoundingBox;
}
