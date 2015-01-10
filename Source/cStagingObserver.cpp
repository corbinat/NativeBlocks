#include "cStagingObserver.h"
#include "cResources.h"
#include <iostream>

cStagingObserver::cStagingObserver(cResources* a_pResources, std::minstd_rand a_RandomNumberEngine, std::string a_Identifier)
   : cObject(a_pResources),
     m_Initialized(false),
     m_Staging(GetResources(), a_RandomNumberEngine, GetUniqueId()),
     m_Identifier(a_Identifier),
     a_pPivotBean(NULL),
     a_pSwingBean(NULL)
{
   // Receive messages when the player needs new beans
   sMessage l_Request;
   l_Request.m_From = GetResources()->GetMessageDispatcher()->AnyID();
   l_Request.m_Category = m_Identifier;
   l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
   l_Request.m_Value = "NewBean";

   std::function<void(sMessage)> l_MessageCallback =
      std::bind(&cStagingObserver::MessageReceived, this, std::placeholders::_1);

   GetResources()->GetMessageDispatcher()->RegisterForMessages(
      GetUniqueId(),
      l_MessageCallback,
      l_Request
      );

}

cStagingObserver::~cStagingObserver()
{
   GetResources()->GetMessageDispatcher()->CancelMessages(GetUniqueId());
}

// These functions are overloaded from cObject
void cStagingObserver::Event(std::list<sf::Event> * a_pEventList)
{
}

void cStagingObserver::Step (uint32_t a_ElapsedMiliSec)
{
   if (!m_Initialized)
   {
      _GetNewBeans();
      m_Initialized = true;
   }
}

void cStagingObserver::Collision(cObject* a_pOther)
{
}

void cStagingObserver::MessageReceived(sMessage a_Message)
{
   if (a_Message.m_Value == "NewBean" && a_Message.m_Category == m_Identifier)
   {
      _GetNewBeans();
   }
}


void cStagingObserver::_GetNewBeans()
{
   if (a_pPivotBean != NULL)
   {
      a_pPivotBean->UnregisterObject(true);
   }
   if (a_pSwingBean != NULL)
   {
      a_pSwingBean->UnregisterObject(true);
   }

   a_pPivotBean = m_Staging.GetNextBean();

   a_pSwingBean = m_Staging.GetNextBean();

   sf::Vector2<uint32_t>* l_pGridCellSize =
      GetResources()->GetActiveLevelData()->GetGridCellSize();

   sf::Vector3<double> l_Position = GetPosition();
   a_pSwingBean->SetPosition(l_Position, kNormal, false);
   l_Position.y += l_pGridCellSize->y;
   a_pPivotBean->SetPosition(l_Position, kNormal, false);


}
