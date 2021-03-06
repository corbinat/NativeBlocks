
#ifndef ___cButton_h___
#define ___cButton_h___

#include "cObject.h"
#include "cTextBox.h"

class cButton: public cObject
{
public:

   cButton(cResources* a_pResources);
   ~cButton();

   // These functions are overloaded from cObject
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step (uint32_t a_ElapsedMiliSec);
   void Draw(const sf::Vector2<float> & a_rkInterpolationOffset);

   void SetImage(std::string a_AniFile, std::string a_Sprite);
   void SetPressedImage(std::string a_AniFile, std::string a_Sprite);

   void SetLabel(std::string a_String, uint32_t a_Size = 21);

private:

   // Pairs contain ani file and ani name
   std::pair<std::string, std::string> m_SpriteImage;
   std::pair<std::string, std::string> m_SpritePressedImage;

   sf::Text m_Label;

};

#endif
