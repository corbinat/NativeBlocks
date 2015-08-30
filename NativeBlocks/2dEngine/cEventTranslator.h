#ifndef ___cEventTranslator_h___
#define ___cEventTranslator_h___

#include "SFML/Graphics.hpp"
#include "cVFileNode.h"

// The cVFileNode format is the following. ClassExclusive is built in and
// indicates if a binding should remove bindings to other types of devices.
// For example, if setting a key binding should remove mouse and joystick
// bindings for that action.
//
// Bindings:
// {
//    ClassExclusive: True
//    LeftAction:
//    {
//       Key: a
//       Mouse: LMB
//       Joystick: Joy1Button1
//    }
//    RightAction:
//    {
//       Key: RightArrowKey
//       Mouse: RMB
//       Joystick: Joy1Button2
//    }
// }

class cEventTranslator
{
public:
   cEventTranslator();
   ~cEventTranslator();

   // This function expects the file to have a "Bindings" class as a top level
   // item.
   bool LoadFromFile(const std::string& a_rkFileName);
   bool SaveToFile(const std::string&  a_rkFileName);

   bool AddBinding(sf::Event a_Event, std::string a_Action);
   bool AddBinding(sf::Keyboard::Key a_Key, std::string a_Action);
   bool DoesConflictExist(const sf::Event& a_rkEvent);
   bool Compare(sf::Event a_Event, std::string a_Action);
   void SetClassExclusive(bool a_ClassExclusive);

   std::string GetActionToEventString(std::string a_Action);


private:

   // TODO: Maybe allow clients to register which events they need defined. We
   // can then use this list to show a keybinding page. They can also provide
   // a default keybinding maybe.

   std::string _EventToString(sf::Event a_Event);
   std::string _KeyToString(sf::Keyboard::Key a_Key);
   std::string _EventCategoryString(sf::Event a_Event);

   cVFileNode m_Bindings;


};

#endif
