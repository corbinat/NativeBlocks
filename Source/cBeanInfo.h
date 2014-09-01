// This class is used by the AI to evaluage the board

#ifndef ___cBeanInfo_h___
#define ___cBeanInfo_h___

#include <unordered_set>

// sf::Vector2
#include "SFML/System.hpp"

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

class cBeanInfo
{
public:

   // This constructor creates a random bean color
   cBeanInfo();

   // This constructor sets the bean color
   cBeanInfo(eBeanColor a_Color);

   virtual ~cBeanInfo();

   eBeanColor GetColor();

   void AddConnection(cBeanInfo* a_pOtherBean);

   std::unordered_set<cBeanInfo*> CountConnections();

   void RemoveAllConnections();

   void SetGridPosition(sf::Vector2<uint32_t> a_GridPosition);
   void SetRowPosition(uint32_t a_Row);

   sf::Vector2<uint32_t> GetGridPosition();


private:

   void _CountConnections(std::unordered_set<cBeanInfo*>* a_ExcludeList);

   eBeanColor m_Color;

   std::unordered_set<cBeanInfo*> m_ConnectedBeans;

   sf::Vector2<uint32_t> m_GridPosition;

};

#endif
