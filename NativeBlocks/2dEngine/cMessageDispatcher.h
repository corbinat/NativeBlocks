// This class is to be owned by the manager and shared with the objects or what ends
// up being the AI brain.
#ifndef ___cMessageDispatcher_h___
#define ___cMessageDispatcher_h___

#include <stdint.h>
#include <string>
#include <vector>
#include <functional>

struct sMessage
{
   // The ID from which you want to receive messages. Same as m_ID when posting
   // messages. If value is cMessageDispatcher ID then it is any.
   int32_t m_From;

   // The category of the message. Useful if you want to receive messages from
   // a subset of objects
   std::string m_Category;

   // The message key.
   std::string m_Key;

   // The value of the message
   std::string m_Value;
};

struct sRequestedMessage
{
   // The ID of the object requesting messages (To be used when
   // canceling requested messages).
   int32_t m_ID;

   // Which function to call on message
   std::function<void(sMessage)> m_Function;

   sMessage m_Message;

};

class cMessageDispatcher
{
public:

   cMessageDispatcher(int32_t a_ID);
   ~cMessageDispatcher();

   // Post a message to the dispatcher
   void PostMessage(sMessage a_Message);

   // Register to receive messages that meet criteria
   void RegisterForMessages(
      int32_t a_ID,
      std::function<void(sMessage)> a_Callback,
      sMessage a_Message
      );

   // Unregister to receive messages
   void CancelMessages(int32_t a_ID);

   // Wildcard Placeholders
   int32_t AnyID();
   std::string Any();

private:

   void _FlushChanges();

   int32_t m_ID;

   const std::string m_StringWildcard;

   std::vector<sRequestedMessage> m_RequestedMessages;
   std::vector<sRequestedMessage> m_AddQueue;
   std::vector<int32_t> m_RemoveQueue;

   // This value is incremented when posting messages. When sending out messages
   // we are looping through the vector and therefore it isn't safe to add or
   // remove elements. If something tries to add or remove then it will just get
   // saved to the Add or Remove queue. When NestedCount goes to 0 we will sync
   // those changes with the main vector.
   uint32_t m_NestedCount;
};

#endif
