#include "cSlider.h"

cSliderNub::cSliderNub(cResources* a_pResources)
   : cObject(a_pResources)
{
   SetType("cSliderNub");
   SetSolid(false);
   SetCollidable(false);
   LoadAnimations("Media/Slider.ani");
   PlayAnimationLoop("SliderNub");
}

cSliderNub::~cSliderNub()
{
}

// These functions are overloaded from cObject
void cSliderNub::Initialize()
{
}
void cSliderNub::Collision(cObject* a_pOther)
{
}
void cSliderNub::Event(std::list<sf::Event> * a_pEventList)
{
}
void cSliderNub::Step(uint32_t a_ElapsedMiliSec)
{
}
void cSliderNub::Draw(const sf::Vector2<float> & a_rkInterpolationOffset)
{
}

cSlider::cSlider(cResources* a_pResources)
   : cObject(a_pResources),
     m_Callback(),
     m_pSliderNub(NULL),
     m_Active(false),
     m_Value(0)
{
   SetType("cSlider");
   SetSolid(false);
   SetCollidable(false);
   LoadAnimations("Media/Slider.ani");
   PlayAnimationLoop("Slider");

   m_pSliderNub = new cSliderNub(a_pResources);

   AddChild(m_pSliderNub);
}

cSlider::~cSlider()
{
}

// These functions are overloaded from cObject
void cSlider::Initialize()
{
   m_pSliderNub->SetPosition(GetPosition(), kNormal, false);
   m_pSliderNub->Initialize();
}

void cSlider::Collision(cObject* a_pOther)
{

}
void cSlider::Event(std::list<sf::Event> * a_pEventList)
{
   for (std::list<sf::Event>::iterator i = a_pEventList->begin();
      i != a_pEventList->end();
      ++i
      )
   {
      switch((*i).type)
      {
         case sf::Event::MouseButtonPressed:
         {
            sf::Rect<int32_t> l_BoundingBox = GetBoundingBox();
            l_BoundingBox.left = GetPosition().x;
            l_BoundingBox.top = GetPosition().y;

            if (l_BoundingBox.contains(i->mouseButton.x, i->mouseButton.y))
            {
               m_Active = true;
               _MoveNubToMouse(i->mouseButton.x);
            }

            break;
         }
         case sf::Event::MouseButtonReleased:
         {
            if (m_Active)
            {
               PlaySound("Media/Sounds/Click1.ogg");
            }
            m_Active = false;
            break;
         }
         case sf::Event::MouseMoved:
         {
            if (m_Active)
            {
               _MoveNubToMouse(i->mouseMove.x);
            }
            break;
         }
         default:
         {
            break;
         }
      }
   }
}

void cSlider::Step(uint32_t a_ElapsedMiliSec)
{
   if (GetPreviousPosition() != GetPosition())
   {
      SetValue(m_Value);
   }
}

void cSlider::Draw(const sf::Vector2<float> & a_rkInterpolationOffset)
{
}

void cSlider::RegisterCallback(std::function<void(cObject*)> a_Callback)
{
   m_Callback = a_Callback;
}

void cSlider::SetValue(double a_Value)
{
   m_Value = a_Value;
   sf::Vector3<double> l_Position = GetPosition();

   l_Position.x += (m_Value / 100.0) * (GetBoundingBox().width - m_pSliderNub->GetBoundingBox().width);

   m_pSliderNub->SetPosition(l_Position, kNormal, false);
}

void cSlider::_MoveNubToMouse(int32_t a_MouseX)
{
   double l_NubHalfWidth = m_pSliderNub->GetBoundingBox().width / 2;

   if (a_MouseX - l_NubHalfWidth < GetPosition().x)
   {
      m_pSliderNub->SetPosition(GetPosition(), kNormal, false);
   }
   else if(a_MouseX + l_NubHalfWidth > GetPosition().x + GetBoundingBox().width)
   {
      sf::Vector3<double> l_Position = GetPosition();
      l_Position.x += GetBoundingBox().width - m_pSliderNub->GetBoundingBox().width;
      m_pSliderNub->SetPosition(l_Position, kNormal, false);
   }
   else
   {
      sf::Vector3<double> l_Position = GetPosition();
      l_Position.x = a_MouseX - l_NubHalfWidth;
      m_pSliderNub->SetPosition(l_Position, kNormal, false);
   }

   // Update the value
   m_Value =
      (m_pSliderNub->GetPosition().x - GetPosition().x)
        / (GetBoundingBox().width - m_pSliderNub->GetBoundingBox().width)
        * 100;

   // Let everyone know what has happened here
   sMessage l_Message;
   l_Message.m_From = GetUniqueId();
   l_Message.m_Category = "Widget";
   l_Message.m_Key = "Slider";
   l_Message.m_Value = std::to_string(m_Value);;
   GetResources()->GetMessageDispatcher()->PostMessage(l_Message);

}
