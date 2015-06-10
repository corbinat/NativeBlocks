
#ifndef ___cHighScoresList_h___
#define ___cHighScoresList_h___

#include "cObject.h"

struct sHighScore
{
   sf::Text m_Name;
   sf::Text m_Level;
   sf::Text m_Retries;

   sHighScore()
      : m_Name(),
        m_Level(),
        m_Retries()
   {
      m_Name.setCharacterSize(20);
      m_Name.setColor(sf::Color::Black);
      m_Name.setString("<Blank>");

      m_Level.setCharacterSize(20);
      m_Level.setColor(sf::Color::Black);
      m_Level.setString("0");

      m_Retries.setCharacterSize(20);
      m_Retries.setColor(sf::Color::Black);
      m_Retries.setString("0");
   }
};

class cHighScoresList: public cObject
{
public:

   cHighScoresList(cResources* a_pResources);
   ~cHighScoresList();

   // These functions are overloaded from cObject
   void Initialize();
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step (uint32_t a_ElapsedMiliSec);
   void Draw();

private:
   void _EvaluateHighScoreList();

   sf::Text m_Title;
   std::vector<sHighScore> m_HighScores;
   sf::Text m_ContinueString;

};

#endif
