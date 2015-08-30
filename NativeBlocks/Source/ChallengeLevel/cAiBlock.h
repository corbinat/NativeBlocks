#ifndef ___cAiBlock_h___
#define ___cAiBlock_h___

#include "cObject.h"


class cAiBlock: public cObject
{
public:

   cAiBlock(cResources* a_pResources);
   ~cAiBlock();

   void SetAiLabel(std::string a_Label);
   void Fall();

   // These functions are overloaded from cObject
   void Initialize();
   void Event(std::list<sf::Event> * a_pEventList);
   void Step(uint32_t a_ElapsedMiliSec);
   void Collision(cObject* a_pOther);
   void Draw(const sf::Vector2<float> & a_rkInterpolationOffset);


private:
   sf::Text m_AiLabel;
   bool m_Falling;

};

#endif
