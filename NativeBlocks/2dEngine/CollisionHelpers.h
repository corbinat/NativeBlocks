// These are helper functions to assist with cObject collision detection and
// response

#ifndef ___CollisionHelpers_h___
#define ___CollisionHelpers_h___

#include "SFML/Graphics.hpp"

// This function returns how much of a shift is needed for a point to touch a
// line.
double ShiftPointXToLine(
   sf::Vector2<double> a_Point,
   sf::Vector2<double> a_LineP1,
   sf::Vector2<double> a_LineP2
   );

double ShiftPointYToLine(
   sf::Vector2<double> a_Point,
   sf::Vector2<double> a_LineP1,
   sf::Vector2<double> a_LineP2
   );

#endif
