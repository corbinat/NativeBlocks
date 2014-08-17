
#ifndef ___cBean_h___
#define ___cBean_h___

#include "cObject.h"

#include <unordered_set>

enum eBeanColor
{
   kBeanColorBlue,
   kBeanColorGreen,
   kBeanColorYellow,
   kBeanColorRed,
   kBeanColorPink,
   kBeanColorNumber, // Used to identify how many colors are possible
   kBeanColorGarbage
};

class cBean: public cObject
{
public:

   // This constructor creates a random bean color
   cBean(cResources* a_pResources);

   // This constructor sets the bean color
   cBean(eBeanColor a_Color, cResources* a_pResources);

   virtual ~cBean();

   // These functions are overloaded from cObject
   virtual void Event(std::list<sf::Event> * a_pEventList);
   virtual void Step(uint32_t a_ElapsedMiliSec);
   virtual void Collision(cObject* a_pOther);

   void Fall();
   eBeanColor GetColor();

   void AddConnection(cBean* a_pOtherBean);

   std::unordered_set<cBean*> CountConnections();

   void Explode();
   bool IsExploding();

private:

   void _CountConnections(std::unordered_set<cBean*>* a_ExcludeList);

   eBeanColor m_Color;

   // Whether this bean should be moved by gravity
   bool m_FreeFall;

   // If the beans have entered play (not in staging)
   bool m_InPlay;

   std::unordered_set<cBean*> m_ConnectedBeans;

   bool m_Exploding;

};

#endif
