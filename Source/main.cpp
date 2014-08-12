// sudo ldconfig /home/corbin/Documents/c++/projects/External/SFML-2.0-rc/lib/
#include "cBeanLevel.h"
//#include "cMainMenuLevel.h"

#include "cResources.h"
#include "cObject.h"
#include "cLevelManager.h"

int main(int argc, char* argv[])
{
   cResources l_Resources;
   l_Resources.CreateWindow(800, 600, "blarg");

   //~ cMainMenuLevel l_MainMenu(
      //~ "Media/levels.lvl",
      //~ "MainMenu",
      //~ &l_Resources
      //~ );

   cBeanLevel l_MainLevel(
      "Media/levels.lvl",
      "Level1",
      &l_Resources
      );

   l_Resources.SetActiveLevel("Level1", false);

   cLevelManager l_LevelManager(&l_Resources);
   l_LevelManager.AddLevel(&l_MainLevel);
   //l_LevelManager.AddLevel(&l_MainMenu);

   while (l_LevelManager.IsRunning())
   {
      l_LevelManager.Update();
   }
   return 0;
}
