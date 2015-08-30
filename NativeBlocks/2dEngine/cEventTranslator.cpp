#include "cEventTranslator.h"

#include <iostream>

static const std::string g_kKeyCategoryString = "Key";
static const std::string g_kMouseCategoryString = "Mouse";
static const std::string g_kJoystickCategoryString = "Joystick";


cEventTranslator::cEventTranslator()
   : m_Bindings()
{
}

cEventTranslator::~cEventTranslator()
{
}

bool cEventTranslator::LoadFromFile(const std::string&  a_rkFileName)
{
   cVFileNode l_Bindings;
   l_Bindings.LoadFile(a_rkFileName);
   m_Bindings = l_Bindings["Bindings"][0];

   if (m_Bindings.Size() == 0)
   {
      return false;
   }
   return true;
}

bool cEventTranslator::SaveToFile(const std::string&  a_rkFileName)
{
   return m_Bindings.ExportToFile(a_rkFileName);
}

bool cEventTranslator::AddBinding(sf::Event a_Event, std::string a_Action)
{
   std::string a_Category = _EventCategoryString(a_Event);
   std::string a_EventString = _EventToString(a_Event);
   if (a_Category.empty() || a_EventString.empty())
   {
      return false;
   }

   if (static_cast<std::string>(m_Bindings["ClassExclusive"]) == "True")
   {
      m_Bindings[a_Action]["Key"] = "";
      m_Bindings[a_Action]["Mouse"] = "";
      m_Bindings[a_Action]["Joystick"] = "";
   }

   m_Bindings[a_Action][a_Category] = a_EventString;

   return true;
}

// Convenience function so that I can hardcode some bindings. Should probably
// make one for mouse and joystick too
bool cEventTranslator::AddBinding(sf::Keyboard::Key a_Key, std::string a_Action)
{
   std::string a_Category = g_kKeyCategoryString;
   std::string a_EventString = _KeyToString(a_Key);
   if (a_Category.empty() || a_EventString.empty())
   {
      return false;
   }

   if (static_cast<std::string>(m_Bindings["ClassExclusive"]) == "True")
   {
      m_Bindings[a_Action]["Key"] = "";
      m_Bindings[a_Action]["Mouse"] = "";
      m_Bindings[a_Action]["Joystick"] = "";
   }

   m_Bindings[a_Action][a_Category] = a_EventString;

   return true;
}

bool cEventTranslator::DoesConflictExist(const sf::Event& a_rkEvent)
{
   return false;
}

bool cEventTranslator::Compare(sf::Event a_Event, std::string a_Action)
{
   std::string l_Category = _EventCategoryString(a_Event);
   std::string l_EventString = _EventToString(a_Event);
   if (l_Category.empty() || l_EventString.empty())
   {
      return false;
   }

   std::string l_ActionQuery = m_Bindings[a_Action][l_Category];

   return l_ActionQuery == l_EventString;
}

void cEventTranslator::SetClassExclusive(bool a_ClassExclusive)
{
   if (a_ClassExclusive)
   {
      m_Bindings["ClassExclusive"] = "True";
   }
   else
   {
      m_Bindings["ClassExclusive"] = "False";
   }
}

std::string cEventTranslator::GetActionToEventString(std::string a_Action)
{
   // Allow users to pass in which category they want
   std::string l_EventString = m_Bindings[a_Action][g_kKeyCategoryString];
   if (!l_EventString.empty())
   {
      return l_EventString;
   }

   l_EventString = m_Bindings[a_Action][g_kMouseCategoryString];
   if (!l_EventString.empty())
   {
      return l_EventString;
   }

   l_EventString = m_Bindings[a_Action][g_kJoystickCategoryString];
   if (!l_EventString.empty())
   {
      return l_EventString;
   }

   return l_EventString;
}

std::string cEventTranslator::_EventToString(sf::Event a_Event)
{
   switch(a_Event.type)
   {
      case sf::Event::KeyPressed:
      case sf::Event::KeyReleased:
      {
         return _KeyToString(a_Event.key.code);
      }
      case sf::Event::MouseButtonPressed:
      {
         return "";
      }
      case sf::Event::JoystickButtonPressed:
      case sf::Event::JoystickButtonReleased:
      {
         return std::string("Joy") + std::to_string(a_Event.joystickButton.joystickId) + ": " + std::to_string(a_Event.joystickButton.button);
      }
      default:
      {
         return "";
      }
   }

   return "";
}

std::string cEventTranslator::_KeyToString(sf::Keyboard::Key a_Key)
{
   if (a_Key <= sf::Keyboard::Z
      && a_Key >= sf::Keyboard::A
      )
   {
      // ASCII 65 is A
      char l_Key =
          static_cast<char>(a_Key) + static_cast<char>(65);

      return std::string(1, l_Key);
   }
   else if (a_Key >= sf::Keyboard::Num0
     && a_Key <= sf::Keyboard::Num9
     )
   {
      // ASCII 48 is 0
      char l_Key =
         static_cast<char>(a_Key) - sf::Keyboard::Num0 + static_cast<char>(48);

      return std::string(1, l_Key);
   }

   switch (a_Key)
   {
      case sf::Keyboard::Down:
         return "Down";
      case sf::Keyboard::Up:
         return "Up";
      case sf::Keyboard::Left:
         return "Left";
      case sf::Keyboard::Right:
         return "Right";
      case sf::Keyboard::Space:
         return "Space";
      case sf::Keyboard::Escape:
         return "Escape";
      default:
         return "";

   }
}

std::string cEventTranslator::_EventCategoryString(sf::Event a_Event)
{
   switch(a_Event.type)
   {
      case sf::Event::KeyPressed:
      case sf::Event::KeyReleased:
      {
         return g_kKeyCategoryString;
      }
      case sf::Event::MouseButtonPressed:
      {
         return g_kMouseCategoryString;
      }
      case sf::Event::JoystickButtonPressed:
      case sf::Event::JoystickButtonReleased:
      {
         return g_kJoystickCategoryString;
      }
      default:
      {
         return "";
      }
   }
}
