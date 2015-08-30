
#ifndef ___cCreditsString_h___
#define ___cCreditsString_h___

#include "cObject.h"

class cSfmlLogo;

class cCreditsString: public cObject
{
public:

   cCreditsString(cResources* a_pResources);
   ~cCreditsString();

   // These functions are overloaded from cObject
   void Initialize();
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step (uint32_t a_ElapsedMiliSec);
   void Draw(const sf::Vector2<float> & a_rkInterpolationOffset);

private:

   sf::Text m_CreditText;
   sf::Text m_CreditText2;
   sf::Text m_ContinueString;

   cSfmlLogo * m_pSfmlLogo;

};

#endif
