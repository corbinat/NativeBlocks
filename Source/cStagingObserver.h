#ifndef ___cStagingObserver_h___
#define ___cStagingObserver_h___

#include "cObject.h"
#include "cStaging.h"
#include "cMessageDispatcher.h"

class cBean;

class cStagingObserver: public cObject
{
public:

   cStagingObserver(cResources* a_pResources, std::minstd_rand a_RandomNumberEngine, std::string a_Identifier);
   virtual ~cStagingObserver();

   // These functions are overloaded from cObject
   virtual void Event(std::list<sf::Event> * a_pEventList);
   virtual void Step (uint32_t a_ElapsedMiliSec);
   virtual void Collision(cObject* a_pOther);

   void MessageReceived(sMessage a_Message);

private:

   void _GetNewBeans();

   bool m_Initialized;

   cStaging m_Staging;

   std::string m_Identifier;

   cBean* a_pPivotBean;
   cBean* a_pSwingBean;

};

#endif
