#include "CollisionHelpers.h"

double ShiftPointXToLine(
   sf::Vector2<double> a_Point,
   sf::Vector2<double> a_LineP1,
   sf::Vector2<double> a_LineP2
   )
{
   // See if the point even has the same y value as a spot on the line
   if (  (a_Point.y > a_LineP1.y && a_Point.y > a_LineP2.y)
      || (a_Point.y < a_LineP1.y && a_Point.y < a_LineP2.y)
      )
   {
      return INFINITY;
   }

   // Test for Verticle line
   if ((a_LineP2.x - a_LineP1.x) == 0)
   {
      return a_LineP1.x - a_Point.x;
   }
   else
   {
      double l_Slope = (a_LineP2.y - a_LineP1.y) / (a_LineP2.x - a_LineP1.x);
      double l_Intercept = a_LineP1.y - l_Slope * a_LineP1.x;

      double l_X = (a_Point.y - l_Intercept) / l_Slope;

      return l_X - a_Point.x;
   }
}

double ShiftPointYToLine(
   sf::Vector2<double> a_Point,
   sf::Vector2<double> a_LineP1,
   sf::Vector2<double> a_LineP2
   )
{
   // See if the point even has the same x value as a spot on the line
   if (  (a_Point.x > a_LineP1.x && a_Point.x > a_LineP2.x)
      || (a_Point.x < a_LineP1.x && a_Point.x < a_LineP2.x)
      )
   {
      return INFINITY;
   }

   // Test for Verticle line
   if ((a_LineP2.x - a_LineP1.x) == 0)
   {
      return fabs(a_LineP1.y - a_Point.x) < fabs(a_LineP2.y - a_Point.x) ?
             (a_LineP1.y - a_Point.x) : (a_LineP2.y - a_Point.x);
   }
   else
   {
      double l_Slope = (a_LineP2.y - a_LineP1.y) / (a_LineP2.x - a_LineP1.x);
      double l_Intercept = a_LineP1.y - l_Slope * a_LineP1.x;

      double l_Y = l_Slope * a_Point.x + l_Intercept;

      return l_Y - a_Point.y;
   }
}
