
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
   void Draw();

   void SetSprite(std::string a_Sprite, std::string a_SpritePressed);

private:
   std::string m_SpriteImage;
   std::string m_SpritePressedImage;

};

#endif
