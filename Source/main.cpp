// sudo ldconfig /home/corbin/Documents/c++/projects/External/SFML-2.2/lib/
#include "cBeanLevel.h"
#include "cMainMenuLevel.h"
#include "ChallengeLevel/cChallengeLevel.h"
#include "HighScores/cHighScoresLevel.h"
#include "Credits/cCreditsLevel.h"

#include "Common/Constants.h"

#include "cResources.h"
#include "cObject.h"
#include "cLevelManager.h"

int main(int argc, char* argv[])
{
   cResources l_Resources;
   l_Resources.CreateWindow(800, 600, "Native Blocks");
   l_Resources.AddAssetResourcePack("Media/Media.pak");

   //~ bool l_Success =
      //~ l_Resources.GetEventTranslator()->LoadFromFile("Config/EventBindings.bnd");

   l_Resources.GetEventTranslator()->AddBinding(sf::Keyboard::Down, g_kActionDown);
   l_Resources.GetEventTranslator()->AddBinding(sf::Keyboard::Left, g_kActionLeft);
   l_Resources.GetEventTranslator()->AddBinding(sf::Keyboard::Right, g_kActionRight);
   l_Resources.GetEventTranslator()->AddBinding(sf::Keyboard::Z, g_kActionRotateCounterClockwise);
   l_Resources.GetEventTranslator()->AddBinding(sf::Keyboard::X, g_kActionRotateClockwise);
   l_Resources.GetEventTranslator()->AddBinding(sf::Keyboard::Escape, g_kActionPause);

   //~ if (!l_Success)
   //~ {
      //~ std::cout << "Failed to load controls from Config/EventBindings.bnd" << std::endl;
      //~ return 0;
   //~ }

   cMainMenuLevel l_MainMenu(&l_Resources);
   cBeanLevel l_MainLevel(&l_Resources);
   cChallengeLevel l_ChallengeLevel(&l_Resources);
   cHighScoresLevel l_HighScoresLevel(&l_Resources);
   cCreditsLevel l_CreditsLevel(&l_Resources);

   //l_Resources.SetActiveLevel("Level1", false);
   l_Resources.SetActiveLevel("MainMenu", false);

   cLevelManager l_LevelManager(&l_Resources);
   l_LevelManager.AddLevel(&l_MainLevel);
   l_LevelManager.AddLevel(&l_MainMenu);
   l_LevelManager.AddLevel(&l_ChallengeLevel);
   l_LevelManager.AddLevel(&l_HighScoresLevel);
   l_LevelManager.AddLevel(&l_CreditsLevel);

   while (l_LevelManager.IsRunning())
   {
      l_LevelManager.Update();
   }
   return 0;
}
