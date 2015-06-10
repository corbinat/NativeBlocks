#ifndef ___cHighScoreRecorder_h___
#define ___cHighScoreRecorder_h___

#include "cObject.h"

class cHighScoreRecorder: public cObject
{
public:

   cHighScoreRecorder(cResources* a_pResources);
   virtual ~cHighScoreRecorder();

   // These functions are overloaded from cObject
   virtual void Initialize();
   virtual void Event(std::list<sf::Event> * a_pEventList);
   virtual void Step (uint32_t a_ElapsedMiliSec);
   virtual void Collision(cObject* a_pOther);
   virtual void Draw();


private:
   sf::Text m_Description;
   sf::Text m_Name;

};

#endif
