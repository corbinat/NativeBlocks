#include "cBean.h"
#include "cResources.h"
#include "cMessageDispatcher.h"

#include <random>
#include <iostream>
#include <chrono>

cBean::cBean(cResources* a_pResources, uint32_t a_ParentId)
   : cObject(a_pResources),
   m_Color(kBeanColorBlue),
   m_GlowLevel(0),
   m_FreeFall(false),
   m_InPlay(false),
   m_ConnectedBeans(),
   m_CachedTotalConnectedBeans(),
   m_Exploding(false),
   m_ParentId(a_ParentId),
   m_Paused(false),
   m_SavedVelocity(),
   m_SavedVisibleState(false)
{
   SetType("Bean");
   SetSolid(true);
   SetCollidable(true);
   LoadAnimations("Media/Beans.ani");

   std::minstd_rand l_Generator(std::chrono::high_resolution_clock::now().time_since_epoch().count());
   std::uniform_int_distribution<int> l_Distribution(0, kBeanColorNumber - 1);

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
         m_Color = kBeanColorOrange;
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

    // Look for messages indicating the game was paused
   sMessage l_PauseRequest;
   l_PauseRequest.m_From = GetResources()->GetMessageDispatcher()->AnyID();
   l_PauseRequest.m_Category = GetResources()->GetMessageDispatcher()->Any();
   l_PauseRequest.m_Key = "PauseState";
   l_PauseRequest.m_Value = GetResources()->GetMessageDispatcher()->Any();

   std::function<void(sMessage)> l_MessageCallback =
      std::bind(&cBean::MessageReceived, this, std::placeholders::_1);

   GetResources()->GetMessageDispatcher()->RegisterForMessages(
      GetUniqueId(),
      l_MessageCallback,
      l_PauseRequest
      );

}

cBean::cBean(eBeanColor a_Color, cResources* a_pResources, uint32_t a_ParentId)
   : cObject(a_pResources),
   m_Color(a_Color),
   m_GlowLevel(0),
   m_FreeFall(false),
   m_InPlay(false),
   m_ConnectedBeans(),
   m_CachedTotalConnectedBeans(),
   m_Exploding(false),
   m_ParentId(a_ParentId),
   m_Paused(false),
   m_SavedVelocity(),
   m_SavedVisibleState(false)
{
   SetType("Bean");
   SetSolid(true);
   LoadAnimations("Media/Beans.ani");
   _SetBaseSprite();

   // These get set to true when the bean gets added to the game
   SetVisible(false);
   SetCollidable(false);

    // Look for messages indicating the game was paused
   sMessage l_PauseRequest;
   l_PauseRequest.m_From = GetResources()->GetMessageDispatcher()->AnyID();
   l_PauseRequest.m_Category = GetResources()->GetMessageDispatcher()->Any();
   l_PauseRequest.m_Key = "PauseState";
   l_PauseRequest.m_Value = GetResources()->GetMessageDispatcher()->Any();

   std::function<void(sMessage)> l_MessageCallback =
      std::bind(&cBean::MessageReceived, this, std::placeholders::_1);

   GetResources()->GetMessageDispatcher()->RegisterForMessages(
      GetUniqueId(),
      l_MessageCallback,
      l_PauseRequest
      );

}

cBean::~cBean()
{
   GetResources()->GetMessageDispatcher()->CancelMessages(GetUniqueId());
}

void cBean::Event(std::list<sf::Event> * a_pEventList)
{

}

void cBean::Step (uint32_t a_ElapsedMiliSec)
{
   if (m_FreeFall && !m_Paused && GetVelocity().y < 1000)
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
         // Continue to freefall if the bean we hit is also still freefalling
         // or exploading
         m_FreeFall = l_pBean->m_FreeFall || l_pBean->m_Exploding;

         // If we hit a bean that is exploding set the velocity back to 0
         if (l_pBean->m_Exploding)
         {
            SetVelocityY(0, kNormal);
         }
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
         l_Message.m_Category = std::to_string(m_ParentId);
         l_Message.m_Key = GetResources()->GetMessageDispatcher()->Any();
         l_Message.m_Value = "BeanSettled";
         GetResources()->GetMessageDispatcher()->PostMessage(l_Message);
      }
   }
}

void cBean::ResetBean()
{
   m_FreeFall = false;
   m_InPlay = false;
   m_ConnectedBeans.clear();
   m_CachedTotalConnectedBeans.clear();
   m_Exploding = false;
   SetVisible(true);
   SetPosition(sf::Vector3<double>{0,0,0}, kNormal, false);
   SetCollidable(true);
}

void cBean::Fall()
{
   m_FreeFall = true;
   _SetBaseSprite();
   for (cBean* l_pBean : m_ConnectedBeans)
   {
      l_pBean->m_ConnectedBeans.erase(this);
   }

   for (cBean* l_pBean : m_CachedTotalConnectedBeans)
   {
      if (l_pBean == this)
      {
         continue;
      }

      // Subtract one from that bean's glow level and set the correct sprite.
      if (l_pBean->m_GlowLevel > 0)
      {
         l_pBean->SetGlowLevel(l_pBean->m_GlowLevel - 1);
         l_pBean->m_CachedTotalConnectedBeans.erase(this);
      }
   }

   m_CachedTotalConnectedBeans.clear();
   m_ConnectedBeans.clear();
}

void cBean::SetColor(eBeanColor a_Color)
{
   m_Color = a_Color;
   _SetBaseSprite();
}

eBeanColor cBean::GetColor()
{
   return m_Color;
}

void cBean::SetGlowLevel(uint32_t a_Level)
{
   m_GlowLevel = a_Level;
   switch(m_Color)
   {
      case kBeanColorOrange:
      {
         if (a_Level == 1)
         {
            PlayAnimationLoop("Red");
         }
         else if (a_Level == 2)
         {
            PlayAnimationLoop("RedGlow1");
         }
         else
         {
            PlayAnimationLoop("RedGlow2");
         }
         break;
      }
      case kBeanColorBlue:
      {
         if (a_Level == 1)
         {
            PlayAnimationLoop("Blue");
         }
         else if (a_Level == 2)
         {
            PlayAnimationLoop("BlueGlow1");
         }
         else
         {
            PlayAnimationLoop("BlueGlow2");
         }
         break;
      }
      case kBeanColorYellow:
      {
         if (a_Level == 1)
         {
            PlayAnimationLoop("Yellow");
         }
         else if (a_Level == 2)
         {
            PlayAnimationLoop("YellowGlow1");
         }
         else
         {
            PlayAnimationLoop("YellowGlow2");
         }
         break;
      }
      case kBeanColorGreen:
      {
         if (a_Level == 1)
         {
            PlayAnimationLoop("Green");
         }
         else if (a_Level == 2)
         {
            PlayAnimationLoop("GreenGlow1");
         }
         else
         {
            PlayAnimationLoop("GreenGlow2");
         }
         break;
      }
      case kBeanColorPink:
      {
         if (a_Level == 1)
         {
            PlayAnimationLoop("Pink");
         }
         else if (a_Level == 2)
         {
            PlayAnimationLoop("PinkGlow1");
         }
         else
         {
            PlayAnimationLoop("PinkGlow2");
         }
         break;
      }
      case kBeanColorGarbage:
      {
         break;
      }
      default:
      {
         UnregisterObject(true);
      }
   }
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
      //~ std::cout << "WHATTTTTTTTT" << std::endl;
      return a_ExcludeList;
   }

   a_ExcludeList.insert(this);

   for (cBean* l_pBean : m_ConnectedBeans)
   {
      l_pBean->_CountConnections(&a_ExcludeList);
   }

   // Use this oportunity to update the bean sprites so that they glow when
   // making a connection. Also update everyones cache.
   m_CachedTotalConnectedBeans = a_ExcludeList;
   m_GlowLevel = a_ExcludeList.size();
   for (cBean* l_pBean : a_ExcludeList)
   {
      l_pBean->SetGlowLevel(m_GlowLevel);
      l_pBean->m_CachedTotalConnectedBeans = m_CachedTotalConnectedBeans;
   }

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

   std::function<void(void)> l_MessageCallback =
      std::bind(&cBean::ExplodeDone, this);

   switch(m_Color)
   {
      case kBeanColorOrange:
      {
         PlayAnimationOnce("RedExplode", l_MessageCallback);
         break;
      }
      case kBeanColorBlue:
      {
         PlayAnimationOnce("BlueExplode", l_MessageCallback);
         break;
      }
      case kBeanColorYellow:
      {
         PlayAnimationOnce("YellowExplode", l_MessageCallback);
         break;
      }
      case kBeanColorGreen:
      {
         PlayAnimationOnce("GreenExplode", l_MessageCallback);
         break;
      }
      case kBeanColorPink:
      {
         PlayAnimationOnce("PinkExplode", l_MessageCallback);
         break;
      }
      case kBeanColorGarbage:
      {
         PlayAnimationOnce("GarbageExplode", l_MessageCallback);
         break;
      }
      default:
      {
         UnregisterObject(true);
      }
   }

   // Don't need to do anything with connected beans because they should be
   // exploding too.
   //UnregisterObject(true);
}

bool cBean::IsExploding()
{
   return m_Exploding;
}

void cBean::ExplodeDone()
{
   SetVisible(false);
   SetCollidable(false);
   SetPosition(sf::Vector3<double>{0,0,0}, kNormal, false);
   m_ConnectedBeans.clear();
   m_CachedTotalConnectedBeans.clear();
   //UnregisterObject(true);
}

void cBean::MessageReceived(sMessage a_Message)
{
   if(a_Message.m_Key == "PauseState")
   {
      if (a_Message.m_Value == "Pause")
      {
         m_Paused = true;
         m_SavedVelocity = GetVelocity();
         SetVelocityY(0, kNormal);
         PauseAnimation(true);
         m_SavedVisibleState = IsVisible();
         SetVisible(false);
      }
      else
      {
         m_Paused = false;
         SetVelocity(m_SavedVelocity, kNormal);
         PauseAnimation(false);
         SetVisible(m_SavedVisibleState);
      }
   }
}

void cBean::_SetBaseSprite()
{
   switch(m_Color)
   {
      case kBeanColorBlue:
      {
         PlayAnimationLoop("Blue");
         break;
      }
      case kBeanColorGreen:
      {
         PlayAnimationLoop("Green");
         break;
      }
      case kBeanColorYellow:
      {
         PlayAnimationLoop("Yellow");
         break;
      }
      case kBeanColorOrange:
      {
         PlayAnimationLoop("Red");
         break;
      }
      case kBeanColorPink:
      {
         PlayAnimationLoop("Pink");
         break;
      }
      default:
      {
         PlayAnimationLoop("Garbage");
      }
   }
}

