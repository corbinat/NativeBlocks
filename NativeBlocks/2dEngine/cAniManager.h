// This class is intended to be used by each cObject. Its pupose is to be in
// charge of animations.

#ifndef ___cAniManager_h___
#define ___cAniManager_h___

#include "cAnimation.h"
#include <map>
#include <memory>

class cResources;
// Each node contains a key->string map for normal properies and a map/vector
// combo for named array properties.
struct cAniFileTree
{
std::map<std::string, std::string> m_Property;

std::map<std::string, cAniFileTree> m_Child;
std::vector<cAniFileTree> m_Vector;
};

class cAniManager
{
public:
   cAniManager(cResources* a_pResources);
   ~cAniManager();

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

   void SetPosition(double a_X, double a_Y);

   // Gets the current animation frame
   sf::Sprite* GetSprite();

   // Gets the current frame's bounding box
   sf::Rect<int32_t> GetBoundingBox();

   // Sets the bouning box. Only used when there is no sprite.
   void SetBoundingBox(sf::Rect<int32_t> a_BoundingBox);

private:
   cResources* m_pResources;
   std::map<std::string, cAnimation> m_AnimationMap;
   std::string m_CurrentAnimation;

   // Used to restore the last animation if the current animation is requested
   // to only run once
   std::string m_LastAnimation;

   // Only used when there is no sprite.
   sf::Rect<int32_t> m_BoundingBox;

   // If PlayAnimationOnce is called, this flag will get set when the animation
   // is over.
   bool m_AnimationEnded;
   bool m_PlayingAnimationOnce;

   // Call this at the end of an animation if it was requested to only play once
   std::function<void(void)> m_Callback;

   bool m_Paused;

};

#endif
