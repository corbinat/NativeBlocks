
#ifndef ___cOptionsMenu_h___
#define ___cOptionsMenu_h___

#include "cObject.h"

class cControlsMenu;
class cButton;
class cSlider;

class cOptionsMenu: public cObject
{
public:

   cOptionsMenu(cResources* a_pResources);
   ~cOptionsMenu();

   void SetActive(bool a_Active);

   // These functions are overloaded from cObject
   void Initialize();
   void Collision(cObject* a_pOther);
   void Event(std::list<sf::Event> * a_pEventList);
   void Step (uint32_t a_ElapsedMiliSec);
   void Draw();

   // Used by child widgets to communicate to this parent widget
   void MessageReceived(sMessage a_Message);

private:
   bool m_Active;

   sf::Text m_SoundLabel;
   sf::Text m_MusicLabel;

   cSlider * m_pSoundSlider;
   cSlider * m_pMusicSlider;
   cButton * m_pKeyMapButton;
   cControlsMenu * m_pControlsMenu;
   cButton * m_pCreditsButton;
   cButton * m_pBackButton;

   bool m_PostBackMessage;

};

#endif
