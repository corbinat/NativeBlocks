#include "cView.h"
#include <iostream>

cView::cView()
   : m_View(),
     m_PaddingX(0),
     m_PaddingY(0),
     m_MinX(0),
     m_MinY(0),
     m_MaxX(0),
     m_MaxY(0),
     m_ObjectId(0)
{
}

cView::~cView()
{
}

void cView::Reset(sf::Rect<float> a_Dimensions)
{
   m_View.reset(a_Dimensions);
}

void cView::SetPadding(uint32_t a_X, uint32_t a_Y)
{
   m_PaddingX = a_X;
   m_PaddingY = a_Y;
}

void cView::SetLimits(
   int32_t a_MinX,
   int32_t a_MinY,
   int32_t a_MaxX,
   int32_t a_MaxY
   )
{
   m_MinX = a_MinX;
   m_MinY = a_MinY;
   m_MaxX = a_MaxX;
   m_MaxY = a_MaxY;
}

// This is so that objects can notify the view that it moved.
void cView::Update(std::vector<sf::Vector2<double>> a_Box)
{
   sf::Vector2<float> l_ViewSize = m_View.getSize();
   sf::Vector2<float> l_ViewCenter = m_View.getCenter();

   double l_LeftMost = a_Box[0].x;
   double l_RightMost = a_Box[0].x;
   double l_TopMost = a_Box[0].y;
   double l_BottomMost = a_Box[0].y;
   for (const auto& i : a_Box)
   {
      if (i.x < l_LeftMost)
      {
         l_LeftMost = i.x;
      }
      if (i.y < l_TopMost)
      {
         l_TopMost = i.y;
      }
      if (i.x > l_RightMost)
      {
         l_RightMost = i.x;
      }
      if (i.y > l_BottomMost)
      {
         l_BottomMost = i.y;
      }
   }

   bool l_NeedUpdate = false;
   float l_NewX = m_View.getCenter().x;
   float l_NewY = m_View.getCenter().y;

   if (m_PaddingX > l_LeftMost - (l_ViewCenter.x - l_ViewSize.x/2))
   {
      l_NewX = l_LeftMost + l_ViewSize.x/2 - m_PaddingX;
      l_NeedUpdate = true;
   }
   if (m_PaddingX > (l_ViewCenter.x + l_ViewSize.x/2) - l_RightMost)
   {
      l_NewX = l_RightMost - l_ViewSize.x/2 + m_PaddingX;
      l_NeedUpdate = true;
   }
   if (m_PaddingY > l_TopMost - (l_ViewCenter.y - l_ViewSize.y/2))
   {
      l_NewY = l_TopMost + l_ViewSize.y/2 - m_PaddingY;
      l_NeedUpdate = true;
   }
   if (m_PaddingY > (l_ViewCenter.y + l_ViewSize.y/2) - l_BottomMost)
   {
      l_NewY = l_BottomMost - l_ViewSize.y/2 + m_PaddingY;
      l_NeedUpdate = true;
   }

   if (l_NeedUpdate)
   {
      // If we're too close to the edge then move back
      if ((l_NewX - l_ViewSize.x/2) < m_MinX)
      {
         l_NewX = m_MinX + l_ViewSize.x/2;
      }
      if ((l_NewX + l_ViewSize.x/2) > m_MaxX)
      {
         l_NewX = m_MaxX - l_ViewSize.x/2;
      }
      if ((l_NewY - l_ViewSize.y/2) < m_MinY)
      {
         l_NewY = m_MinY + l_ViewSize.y/2;
      }
      if ((l_NewY + l_ViewSize.y/2) > m_MaxY)
      {
         l_NewY = m_MaxY - l_ViewSize.y/2;
      }

      m_View.setCenter(
         l_NewX,
         l_NewY
         );
   }

}

