#include "cMessageDispatcher.h"

#include <iostream>

cMessageDispatcher::cMessageDispatcher(int32_t a_ID)
   : m_ID (a_ID),
     m_StringWildcard("cMessageDispatcher::Any"),
     m_RequestedMessages(),
     m_AddQueue(),
     m_RemoveQueue(),
     m_NestedCount(0)
{}

cMessageDispatcher::~cMessageDispatcher()
{}

void cMessageDispatcher::PostMessage(sMessage a_Message)
{
   _FlushChanges();

   ++m_NestedCount;

   // Send the messages
   for (sRequestedMessage i : m_RequestedMessages)
   {
      if (  (i.m_Message.m_From == a_Message.m_From
         || i.m_Message.m_From == AnyID())

         && (i.m_Message.m_Category == a_Message.m_Category
         || i.m_Message.m_Category == Any())

         && (i.m_Message.m_Key == a_Message.m_Key
         || i.m_Message.m_Key == Any())

         && (i.m_Message.m_Value == a_Message.m_Value
         || i.m_Message.m_Value == Any())
         )
      {
         i.m_Function(a_Message);
      }
   }

   --m_NestedCount;
}


void cMessageDispatcher::RegisterForMessages(
   int32_t a_ID,
   std::function<void(sMessage)> a_Callback,
   sMessage a_Message
   )
{
   _FlushChanges();

   sRequestedMessage l_Request;
   l_Request.m_ID = a_ID;
   l_Request.m_Function = a_Callback;
   l_Request.m_Message = a_Message;

   if (m_NestedCount == 0)
   {
      m_RequestedMessages.push_back(l_Request);
   }
   else
   {
      m_AddQueue.push_back(l_Request);
   }
}


void cMessageDispatcher::CancelMessages(int32_t a_ID)
{
   _FlushChanges();

   if (m_NestedCount == 0)
   {
      auto i = m_RequestedMessages.begin();
      while (i != m_RequestedMessages.end())
      {
         if ((*i).m_ID == a_ID)
         {
            i = m_RequestedMessages.erase(i);
         }
         else
         {
            ++i;
         }
      }
   }
   else
   {
      m_RemoveQueue.push_back(a_ID);
   }

}


int32_t cMessageDispatcher::AnyID()
{
   return m_ID;
}

std::string cMessageDispatcher::Any()
{
   return m_StringWildcard;
}

void cMessageDispatcher::_FlushChanges()
{
   // Only flush if it is safe to.
   if (m_NestedCount != 0)
   {
      return;
   }

   // Delete any messages first in the delete queue first
   for (auto & l_ToDelete : m_RemoveQueue)
   {
      auto i = m_RequestedMessages.begin();
      while (i != m_RequestedMessages.end())
      {
         if ((*i).m_ID == l_ToDelete)
         {
            i = m_RequestedMessages.erase(i);
         }
         else
         {
            ++i;
         }
      }
   }
   m_RemoveQueue.clear();

   // Now add new message registrations
   m_RequestedMessages.insert(
      m_RequestedMessages.end(),
      m_AddQueue.begin(),
      m_AddQueue.end()
      );

   m_AddQueue.clear();
}
