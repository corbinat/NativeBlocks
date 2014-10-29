#include "cBeanInfo.h"

#include <random>
#include <iostream>

cBeanInfo::cBeanInfo()
   : m_Color(kBeanColorEmpty),
     m_ConnectedBeans(),
     m_GridPosition()
{
}

cBeanInfo::cBeanInfo(eBeanColor a_Color)
   : m_Color(a_Color),
   m_ConnectedBeans()
{
}

cBeanInfo::~cBeanInfo()
{
}


eBeanColor cBeanInfo::GetColor()
{
   return m_Color;
}

void cBeanInfo::SetColor(eBeanColor a_NewColor)
{
   m_Color = a_NewColor;
}

bool cBeanInfo::AddConnection(cBeanInfo* a_pOtherBean)
{
   for (auto l_BeanPos : m_ConnectedBeans)
   {
      if (a_pOtherBean->GetGridPosition() == l_BeanPos)
      {
         // Bean is already in list, so return false.
         return false;
      }
   }

   m_ConnectedBeans.push_back(a_pOtherBean->GetGridPosition());
   a_pOtherBean->m_ConnectedBeans.push_back(GetGridPosition());

   return true;
}

std::unordered_set<cBeanInfo*> cBeanInfo::CountConnections(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField
   )
{
   std::unordered_set<cBeanInfo*> a_ExcludeList;

   a_ExcludeList.insert(this);

   for (sf::Vector2<uint32_t> l_BeanPosition : m_ConnectedBeans)
   {
      a_rPlayingField[l_BeanPosition.x][l_BeanPosition.y]._CountConnections(a_rPlayingField, &a_ExcludeList);
   }
   //~ for (cBeanInfo* l_pBean : a_ExcludeList)
   //~ {
      //~ std::cout << "\t" << l_pBean->GetPosition().y << std::endl;
   //~ }

   return a_ExcludeList;
}



void cBeanInfo::_CountConnections(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField,
   std::unordered_set<cBeanInfo*>* a_ExcludeList
   )
{
   a_ExcludeList->insert(this);
   for (sf::Vector2<uint32_t> l_BeanPosition : m_ConnectedBeans)
   {
      if (a_ExcludeList->find(&a_rPlayingField[l_BeanPosition.x][l_BeanPosition.y]) == a_ExcludeList->end())
      {
         a_rPlayingField[l_BeanPosition.x][l_BeanPosition.y]._CountConnections(a_rPlayingField, a_ExcludeList);
      }
   }
}

void cBeanInfo::RemoveAllConnections(
   std::vector<std::vector<cBeanInfo>>& a_rPlayingField
   )
{
   // Iterate through all connections
   for (sf::Vector2<uint32_t> l_BeanPosition : m_ConnectedBeans)
   {
      // For each connection, find this bean and remove it from their connection
      for (auto l_Other = a_rPlayingField[l_BeanPosition.x][l_BeanPosition.y].m_ConnectedBeans.begin();
         l_Other != a_rPlayingField[l_BeanPosition.x][l_BeanPosition.y].m_ConnectedBeans.end();
         ++ l_Other
         )
      {
         if (*l_Other == GetGridPosition())
         {
            a_rPlayingField[l_BeanPosition.x][l_BeanPosition.y].m_ConnectedBeans.erase(l_Other);
            break;
         }
      }
   }
   m_ConnectedBeans.clear();
}

void cBeanInfo::SetGridPosition(sf::Vector2<uint32_t> a_GridPosition)
{
   m_GridPosition = a_GridPosition;
}

void cBeanInfo::SetRowPosition(uint32_t a_Row)
{
   m_GridPosition.y = a_Row;
}

void cBeanInfo::SetColumnPosition(uint32_t a_Column)
{
   m_GridPosition.x = a_Column;
}

sf::Vector2<uint32_t> cBeanInfo::GetGridPosition()
{
   return m_GridPosition;
}
