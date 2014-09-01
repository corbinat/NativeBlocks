#include "cBean.h"
#include "cResources.h"
#include "cMessageDispatcher.h"

#include <random>
#include <iostream>

cBean::cBean(cResources* a_pResources)
   : cObject(a_pResources),
   m_Color(kBeanColorBlue),
   m_FreeFall(false),
   m_InPlay(false),
   m_ConnectedBeans(),
   m_Exploding(false)
{
   SetType("Bean");
   SetSolid(true);
   LoadAnimations("Media/Beans.ani");

   std::random_device l_Generator;
   std::uniform_int_distribution<int> l_Distribution(0, kBeanColorNumber - 1);
   //std::uniform_int_distribution<int> l_Distribution(0, kBeanColorNumber - 4);

   int l_Number = l_Distribution(l_Generator);
   l_Number = l_Distribution(l_Generator);
   switch(l_Number)
   {
      case 0:
      {
         PlayAnimationLoop("Blue");
         m_Color = kBeanColorBlue;
         break;
      }
      case 1:
      {
         PlayAnimationLoop("Green");
         m_Color = kBeanColorGreen;
         break;
      }
      case 2:
      {
         PlayAnimationLoop("Yellow");
         m_Color = kBeanColorYellow;
         break;
      }
      case 3:
      {
         PlayAnimationLoop("Red");
         m_Color = kBeanColorRed;
         break;
      }
      case 4:
      {
         PlayAnimationLoop("Pink");
         m_Color = kBeanColorPink;
         break;
      }
      default:
      {
         PlayAnimationLoop("Garbage");
         m_Color = kBeanColorGarbage;
      }
   }

}

cBean::cBean(eBeanColor a_Color, cResources* a_pResources)
   : cObject(a_pResources),
   m_Color(a_Color),
   m_FreeFall(false),
   m_InPlay(false),
   m_ConnectedBeans(),
   m_Exploding(false)
{
   SetType("Bean");
   SetSolid(true);
   LoadAnimations("Media/Beans.ani");
   PlayAnimationLoop("Blue");
}

cBean::~cBean()
{
}

void cBean::Event(std::list<sf::Event> * a_pEventList)
{

}

void cBean::Step (uint32_t a_ElapsedMiliSec)
{
   if (m_FreeFall && GetVelocity().y < 2000)
   {
      SetVelocityY(2 * a_ElapsedMiliSec, kRelative);
   }
}

void cBean::Collision(cObject* a_pOther)
{
   if (m_FreeFall)
   {
      if (a_pOther->GetType() == "Bean" && a_pOther->GetPosition().y > GetPosition().y)
      {
         SitFlush(a_pOther);
         cBean* l_pBean = dynamic_cast<cBean*>(a_pOther);
         m_FreeFall = l_pBean->m_FreeFall;
      }
      else if (a_pOther->GetType() == "Wall")
      {
         std::cout << "What??????????" << std::endl;
      }
      else if (a_pOther->GetType() == "Floor")
      {
         SitFlush(a_pOther);
         m_FreeFall = false;
      }

      // If we aren't free falling any more then let the player know
      if (!m_FreeFall)
      {
         SetVelocityY(0, kNormal);
         sMessage l_Message;
         l_Message.m_From = GetUniqueId();
         l_Message.m_Category = "Player1";
         l_Message.m_Key = GetResources()->GetMessageDispatcher()->Any();
         l_Message.m_Value = "BeanSettled";
         GetResources()->GetMessageDispatcher()->PostMessage(l_Message);
      }
   }
}

void cBean::Fall()
{
   m_FreeFall = true;
   for (cBean* l_pBean : m_ConnectedBeans)
   {
      l_pBean->m_ConnectedBeans.erase(this);
   }
   m_ConnectedBeans.clear();
}

eBeanColor cBean::GetColor()
{
   return m_Color;
}

void cBean::AddConnection(cBean* a_pOtherBean)
{
   m_ConnectedBeans.insert(a_pOtherBean);
   a_pOtherBean->m_ConnectedBeans.insert(this);
}

std::unordered_set<cBean*> cBean::GetImmediateConnections()
{
   return m_ConnectedBeans;
}

std::unordered_set<cBean*> cBean::CountConnections()
{
   std::unordered_set<cBean*> a_ExcludeList;

   if (m_Exploding)
   {
      std::cout << "WHATTTTTTTTT" << std::endl;
      return a_ExcludeList;
   }

   a_ExcludeList.insert(this);

   for (cBean* l_pBean : m_ConnectedBeans)
   {
      l_pBean->_CountConnections(&a_ExcludeList);
   }
   //~ for (cBean* l_pBean : a_ExcludeList)
   //~ {
      //~ std::cout << "\t" << l_pBean->GetPosition().y << std::endl;
   //~ }

   return a_ExcludeList;
}

void cBean::_CountConnections(std::unordered_set<cBean*>* a_ExcludeList)
{
   a_ExcludeList->insert(this);
   for (cBean* l_pBean : m_ConnectedBeans)
   {
      if (a_ExcludeList->find(l_pBean) == a_ExcludeList->end())
      {
         l_pBean->_CountConnections(a_ExcludeList);
      }
   }
}

void cBean::Explode()
{
   m_Exploding = true;
   for (cBean* l_pBean : m_ConnectedBeans)
   {
      l_pBean->m_ConnectedBeans.erase(this);
   }

   UnregisterObject(this, true);
}

bool cBean::IsExploding()
{
   return m_Exploding;
}
