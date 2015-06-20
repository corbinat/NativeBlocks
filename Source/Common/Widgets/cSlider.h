#ifndef ___cSlider_h___
#define ___cSlider_h___

#include "cObject.h"
#include <functional>

class cSliderNub: public cObject
{
public:
   cSliderNub(cResources* a_pResources);
   ~cSliderNub();

   // These functions are overloaded from cObject
   void Initialize();
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step(uint32_t a_ElapsedMiliSec);
   void Draw();

   void SetLimits(double a_Min, double a_Max);

};

class cSlider: public cObject
{
public:

   cSlider(cResources* a_pResources);
   ~cSlider();

   // These functions are overloaded from cObject
   void Initialize();
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step(uint32_t a_ElapsedMiliSec);
   void Draw();

   void RegisterCallback(std::function<void(cObject*)> a_Callback);
   void SetValue(double a_Value);

private:
   void _MoveNubToMouse(int32_t a_MouseX);
   std::function<void(cObject*)> m_Callback;
   cSliderNub * m_pSliderNub;

   // If the mouse is held down
   bool m_Active;

   // Useful if the slider gets moved then we can move the nub to where it needs
   // to be;
   double m_Value;
};


#endif
