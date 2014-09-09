#include "cBeanInfo.h"

#include <random>
#include <iostream>

cBeanInfo::cBeanInfo()
   : m_Color(kBeanColorBlue),
   m_ConnectedBeans(),
   m_GridPosition()
{
   std::random_device l_Generator;
   std::uniform_int_distribution<int> l_Distribution(0, kBeanColorNumber - 1);
   //std::uniform_int_distribution<int> l_Distribution(0, kBeanColorNumber - 4);

   int l_Number = l_Distribution(l_Generator);
   l_Number = l_Distribution(l_Generator);
   switch(l_Number)
   {
      case 0:
      {
         m_Color = kBeanColorBlue;
         break;
      }
      case 1:
      {
         m_Color = kBeanColorGreen;
         break;
      }
      case 2:
      {
         m_Color = kBeanColorYellow;
         break;
      }
      case 3:
      {
         m_Color = kBeanColorRed;
         break;
      }
      case 4:
      {
         m_Color = kBeanColorPink;
         break;
      }
      default:
      {
         m_Color = kBeanColorGarbage;
      }
   }

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

bool cBeanInfo::AddConnection(cBeanInfo* a_pOtherBean)
{
   auto l_Insert = m_ConnectedBeans.insert(a_pOtherBean);
   a_pOtherBean->m_ConnectedBeans.insert(this);

   return l_Insert.second;
}

std::unordered_set<cBeanInfo*> cBeanInfo::CountConnections()
{
   std::unordered_set<cBeanInfo*> a_ExcludeList;

   a_ExcludeList.insert(this);

   for (cBeanInfo* l_pBean : m_ConnectedBeans)
   {
      l_pBean->_CountConnections(&a_ExcludeList);
   }
   //~ for (cBeanInfo* l_pBean : a_ExcludeList)
   //~ {
      //~ std::cout << "\t" << l_pBean->GetPosition().y << std::endl;
   //~ }

   return a_ExcludeList;
}



void cBeanInfo::_CountConnections(std::unordered_set<cBeanInfo*>* a_ExcludeList)
{
   a_ExcludeList->insert(this);
   for (cBeanInfo* l_pBean : m_ConnectedBeans)
   {
      if (a_ExcludeList->find(l_pBean) == a_ExcludeList->end())
      {
         l_pBean->_CountConnections(a_ExcludeList);
      }
   }
}

void cBeanInfo::RemoveAllConnections()
{
   for (cBeanInfo* l_pBean : m_ConnectedBeans)
   {
      l_pBean->m_ConnectedBeans.erase(this);
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

std::unordered_set<cBeanInfo*> cBeanInfo::GetImmediateConnections()
{
   return m_ConnectedBeans;
}
