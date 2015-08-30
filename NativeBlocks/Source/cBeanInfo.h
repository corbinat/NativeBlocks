// This class is used by the AI to evaluage the board

#ifndef ___cBeanInfo_h___
#define ___cBeanInfo_h___

#include <list>
#include <unordered_set>

// sf::Vector2
#include "SFML/System.hpp"

enum eBeanColor
{
   kBeanColorBlue,
   kBeanColorGreen,
   kBeanColorYellow,
   kBeanColorOrange,
   kBeanColorPink,
   kBeanColorNumber, // Used to identify how many colors are possible
   kBeanColorGarbage,
   kBeanColorEmpty    // placeholder for empty space
};

class cBeanInfo
{
public:

   cBeanInfo();

   // This constructor sets the bean color
   cBeanInfo(eBeanColor a_Color);

   virtual ~cBeanInfo();

   eBeanColor GetColor();

   void SetColor(eBeanColor a_NewColor);

   // Returns true if connection was actually made. Used for scoring.
   bool AddConnection(cBeanInfo* a_pOtherBean);

   std::unordered_set<cBeanInfo*> CountConnections(
      std::vector<std::vector<cBeanInfo>>& a_rPlayingField
      );

   void RemoveAllConnections(
      std::vector<std::vector<cBeanInfo>>& a_rPlayingField
      );

   void SetGridPosition(sf::Vector2<uint32_t> a_GridPosition);
   void SetRowPosition(uint32_t a_Row);
   void SetColumnPosition(uint32_t a_Column);

   sf::Vector2<uint32_t> GetGridPosition();

private:

   void _CountConnections(
      std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
      std::unordered_set<cBeanInfo*>* a_ExcludeList
      );

   eBeanColor m_Color;

   std::list<sf::Vector2<uint32_t>> m_ConnectedBeans;

   sf::Vector2<uint32_t> m_GridPosition;

};

#endif
