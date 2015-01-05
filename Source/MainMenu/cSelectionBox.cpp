#include "cSelectionBox.h"
#include "cTextBox.h"
#include "cButton.h"

#include <iostream>

cSelectionBox::cSelectionBox(cResources* a_pResources)
   : cObject(a_pResources),
     m_Initialized(false),
     m_pLeftArrowButton(NULL),
     m_pRightArrowButton(NULL),
     m_pTextBox(NULL),
     m_Options()
{
   SetType("SelectionBox");

   SetSolid(false);

   m_CurrentOption = m_Options.begin();

   m_pLeftArrowButton = new cButton(GetResources());
   m_pTextBox = new cTextBox(GetResources());
   m_pRightArrowButton = new cButton(GetResources());
   m_pRightArrowButton->SetSprite("ButtonRight", "ButtonRightPressed");

   SetBoundingBox(
      sf::Rect<uint32_t>(
         0,
         0,
         m_pLeftArrowButton->GetBoundingBox().width + m_pTextBox->GetBoundingBox().width + m_pRightArrowButton->GetBoundingBox().width + 2,
         m_pTextBox->GetBoundingBox().height
         )
      );
}

cSelectionBox::~cSelectionBox()
{

}

// These functions are overloaded from cObject
void cSelectionBox::Collision(cObject* a_pOther)
{

}

void cSelectionBox::Event(std::list<sf::Event> * a_pEventList)
{
}

void cSelectionBox::Step (uint32_t a_ElapsedMiliSec)
{

   if (!m_Initialized)
   {
      sf::Vector3<double> l_Position = GetPosition();
      m_pLeftArrowButton->SetPosition(l_Position, kNormal, false);

      l_Position.x += m_pLeftArrowButton->GetBoundingBox().width + 1;
      m_pTextBox->SetPosition(l_Position, kNormal, false);

      l_Position.x += m_pTextBox->GetBoundingBox().width + 1;
      m_pRightArrowButton->SetPosition(l_Position, kNormal, false);

      m_Initialized = true;

      // Receive messages when buttons are pushed.
      sMessage l_Request;
      l_Request.m_From = m_pLeftArrowButton->GetUniqueId();
      l_Request.m_Category = GetResources()->GetMessageDispatcher()->Any();
      l_Request.m_Key = GetResources()->GetMessageDispatcher()->Any();
      l_Request.m_Value = GetResources()->GetMessageDispatcher()->Any();

      std::function<void(sMessage)> l_MessageCallback =
         std::bind(&cSelectionBox::MessageReceived, this, std::placeholders::_1);

      GetResources()->GetMessageDispatcher()->RegisterForMessages(
         GetUniqueId(),
         l_MessageCallback,
         l_Request
         );

      l_Request.m_From = m_pRightArrowButton->GetUniqueId();

      GetResources()->GetMessageDispatcher()->RegisterForMessages(
         GetUniqueId(),
         l_MessageCallback,
         l_Request
         );
   }

}

void cSelectionBox::Draw()
{
}

void cSelectionBox::MessageReceived(sMessage a_Message)
{
   if (a_Message.m_From == m_pLeftArrowButton->GetUniqueId())
   {
      std::cout << "Left button pressed" << std::endl;
      if (m_CurrentOption == m_Options.begin())
      {
         m_CurrentOption = m_Options.end() - 1;
      }
      else
      {
         --m_CurrentOption;
      }
      m_pTextBox->ReplaceString(*m_CurrentOption);
   }
   else if (a_Message.m_From == m_pRightArrowButton->GetUniqueId())
   {
      std::cout << "Right button pressed" << std::endl;
      ++m_CurrentOption;
      if (m_CurrentOption == m_Options.end())
      {
         m_CurrentOption = m_Options.begin();
      }
      m_pTextBox->ReplaceString(*m_CurrentOption);
   }
}

void cSelectionBox::SetOptions(std::vector<std::string> & a_rOptions)
{
   m_Options = a_rOptions;
   m_CurrentOption = m_Options.begin();

   if (m_Options.size() > 0)
   {
      m_pTextBox->ReplaceString(*m_CurrentOption);
   }
   else
   {
      m_pTextBox->ReplaceString("");
   }
}

void cSelectionBox::AddOption(const std::string & a_rkOption)
{
   m_Options.push_back(a_rkOption);
   m_CurrentOption = m_Options.begin();
   if (m_Options.size() == 1)
   {
      m_pTextBox->ReplaceString(*m_CurrentOption);
   }
}
