#include "cBean.h"
#include "cResources.h"
#include "cMessageDispatcher.h"

#include <random>

cBean::cBean(cResources* a_pResources)
   : cObject(a_pResources),
   m_Color(kBeanColorBlue),
   m_FreeFall(false),
   m_InPlay(false)
{
   SetType("Bean");
   SetSolid(true);
   LoadAnimations("Media/Beans.ani");

   std::random_device l_Generator;
   std::uniform_int_distribution<int> l_Distribution(0, kBeanColorNumber - 1);

   int l_Number = l_Distribution(l_Generator);
   l_Number = l_Distribution(l_Generator);
   switch(l_Number)
   {
      case 0:
      {
         PlayAnimationLoop("Blue");
         break;
      }
      case 1:
      {
         PlayAnimationLoop("Green");
         break;
      }
      case 2:
      {
         PlayAnimationLoop("Yellow");
         break;
      }
      case 3:
      {
         PlayAnimationLoop("Red");
         break;
      }
      case 4:
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

cBean::cBean(eBeanColor a_Color, cResources* a_pResources)
   : cObject(a_pResources),
   m_Color(a_Color),
   m_FreeFall(false),
   m_InPlay(false)
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
   // Iterate through events
   // TODO: This isn't going to work, for example if we're going right, but the
   // left bean gets moved first it will collide with the right bean. The player
   // should handle moving beans around.

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
      SetVelocityY(0, kNormal);
      SitFlush(a_pOther);
      m_FreeFall = false;

      if (a_pOther->GetType() == "Bean")
      {
         cBean* l_pBean = dynamic_cast<cBean*>(a_pOther);
         if (l_pBean->m_FreeFall)
         {
            m_FreeFall = true;
         }

      }

      // If we aren't free falling any more then let the player know
      if (!m_FreeFall)
      {
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
}
