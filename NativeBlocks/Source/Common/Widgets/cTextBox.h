
#ifndef ___cTextBox_h___
#define ___cTextBox_h___

#include "cObject.h"

class cTextBox: public cObject
{
public:

   cTextBox(cResources* a_pResources);
   ~cTextBox();

   // These functions are overloaded from cObject
   void Initialize();
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step (uint32_t a_ElapsedMiliSec);
   void Draw(const sf::Vector2<float> & a_rkInterpolationOffset);

   void ReplaceString(std::string a_String);

private:

   std::string m_String;
   sf::Text m_Data;
};

#endif
