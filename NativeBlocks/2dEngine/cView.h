#ifndef ___cView_h___
#define ___cView_h___

#include "SFML/Graphics.hpp"

class cView
{
public:
   cView();
   ~cView();

   void Reset(sf::Rect<float> a_Dimensions);

   // This is how close to the edge of the view a followed object can get before
   // the view moves.
   void SetPadding(uint32_t a_X, uint32_t a_Y);

   void SetLimits(
      int32_t a_MinX,
      int32_t a_MinY,
      int32_t a_MaxX,
      int32_t a_MaxY
      );

   // This is so that objects can notify the view that it moved.
   void Update(std::vector<sf::Vector2<double>> a_Box);

   // Allow casting to the sf::view
   operator sf::View()
   {
      return m_View;
   }

   // TODO: Create an attach to object function to replace the ViewFollowObject
   // function in the level manager.

private:
   sf::View m_View;

   // This is how close to the edge of the view a followed object can get before
   // the view moves.
   uint32_t m_PaddingX;
   uint32_t m_PaddingY;

   int32_t m_MinX;
   int32_t m_MinY;
   int32_t m_MaxX;
   int32_t m_MaxY;

   // This is the Last object that sent a message to the view. This can be used
   // to unattach the camera from an object before attaching to another object.
   uint32_t m_ObjectId;
};

#endif
