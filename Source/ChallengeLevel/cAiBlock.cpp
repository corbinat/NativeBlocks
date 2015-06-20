#include "cAiBlock.h"

cAiBlock::cAiBlock(cResources* a_pResources)
   : cObject(a_pResources),
     m_AiLabel(),
     m_Falling(false)
{
   SetType("cAiBlock");

   SetSolid(true);

   std::shared_ptr<sf::Font> l_Font
      = GetResources()->LoadFont("Media/junegull.ttf");
   m_AiLabel.setFont(*(l_Font.get()));
   m_AiLabel.setCharacterSize(40);
   m_AiLabel.setColor(sf::Color::Black);

   LoadAnimations("Media/Challenge.ani");
   PlayAnimationLoop("AiBlock");
}

cAiBlock::~cAiBlock()
{
}

void cAiBlock::SetAiLabel(std::string a_Label)
{
   m_AiLabel.setString(a_Label);
}

void cAiBlock::Fall()
{
   m_Falling = true;
}

void cAiBlock::Initialize()
{
   m_AiLabel.setPosition(
      static_cast<int32_t>(GetPosition().x + GetBoundingBox().width / 2 - m_AiLabel.getLocalBounds().width / 2),
      static_cast<int32_t>(GetPosition().y + GetBoundingBox().height / 2 - m_AiLabel.getCharacterSize() / 2.0 - 10)
      );

}

void cAiBlock::Event(std::list<sf::Event> * a_pEventList)
{

}

void cAiBlock::Step(uint32_t a_ElapsedMiliSec)
{
   if (m_Falling)
   {
      SetVelocityY(1500, kNormal);
      m_AiLabel.setPosition(
         static_cast<int32_t>(GetPosition().x + GetBoundingBox().width / 2 - m_AiLabel.getLocalBounds().width / 2),
         static_cast<int32_t>(GetPosition().y + GetBoundingBox().height / 2 - m_AiLabel.getCharacterSize() / 2.0 - 10)
         );
   }
}

void cAiBlock::Collision(cObject* a_pOther)
{
   if (a_pOther->GetType() == GetType())
   {
      SitFlush(a_pOther);
      m_AiLabel.setPosition(
         static_cast<int32_t>(GetPosition().x + GetBoundingBox().width / 2 - m_AiLabel.getLocalBounds().width / 2),
         static_cast<int32_t>(GetPosition().y + GetBoundingBox().height / 2 - m_AiLabel.getCharacterSize() / 2.0 - 10)
         );
      SetVelocityY(0, kNormal);

      sMessage l_Message;
      l_Message.m_From = GetUniqueId();
      l_Message.m_Category =GetResources()->GetMessageDispatcher()->Any();
      l_Message.m_Key = GetResources()->GetMessageDispatcher()->Any();
      l_Message.m_Value = "Settled";
      GetResources()->GetMessageDispatcher()->PostMessage(l_Message);

      m_Falling = false;

      PlaySound("Media/Sounds/BigFall.ogg");


   }
}

void cAiBlock::Draw()
{
   GetResources()->GetWindow()->draw(m_AiLabel);
}
