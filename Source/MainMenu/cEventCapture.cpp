#include "cEventCapture.h"

#include <iostream>

cEventCapture::cEventCapture(std::string a_Action, std::string a_Label, cResources* a_pResources)
   : cObject(a_pResources),
     m_NewActionLabel(),
     m_Action(a_Action),
     m_Label(a_Label)
{
   SetType("EventCapture");
   SetSolid(false);

   // Depth needs to be low so it is on top of everything else and will receive
   // events first.
   SetDepth(-100);

   std::shared_ptr<sf::Font> l_Font
      = GetResources()->LoadFont("Media/junegull.ttf");

   m_NewActionLabel.setFont(*(l_Font.get()));
   m_NewActionLabel.setString(std::string("Press new button for ") + m_Label + ":");
   m_NewActionLabel.setCharacterSize(20);
   m_NewActionLabel.setColor(sf::Color::Black);

}

cEventCapture::~cEventCapture()
{

}

void cEventCapture::Initialize()
{
   sf::Vector3<double> l_Position = GetPosition();
   m_NewActionLabel.setPosition(GetPosition().x, l_Position.y + 4);
}

// These functions are overloaded from cObject
void cEventCapture::Collision(cObject* a_pOther)
{

}

void cEventCapture::Event(std::list<sf::Event> * a_pEventList)
{
   std::list<sf::Event>::iterator i = a_pEventList->begin();
   for (i; i != a_pEventList->end(); ++i)
   {
      bool l_Success =
         GetResources()->GetEventTranslator()->AddBinding(*i, m_Action);
      if (l_Success)
      {
         sMessage l_Message;
         l_Message.m_From = GetUniqueId();
         l_Message.m_Category = "Update";
         l_Message.m_Key = m_Action;
         l_Message.m_Value = GetResources()->GetEventTranslator()->GetActionToEventString(m_Action);
         GetResources()->GetMessageDispatcher()->PostMessage(l_Message);

         UnregisterObject(true);
         break;
      }
   }

   // Eat all events so that other objects don't react to them
   a_pEventList->clear();
}

void cEventCapture::Step (uint32_t a_ElapsedMiliSec)
{
}

void cEventCapture::Draw()
{
   GetResources()->GetWindow()->draw(m_NewActionLabel);
}
