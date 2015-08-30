#ifndef ___cResources_h___
#define ___cResources_h___

#include "Common.h"
#include "cAnimation.h"
#include "cMessageDispatcher.h"
#include "cView.h"
#include "cVFileNode.h"
#include "cEventTranslator.h"
#include "iLevelData.h"
#include "cPhysFsInputStream.h"

#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

#include <stdint.h> // uint32_t
#include <list>
#include <memory>

// Forward declarations
class cObject;
class cSound;

template<typename T>
bool LoadAsset(T& a_Asset, std::string a_Path, std::shared_ptr<cPhysFsInputStream> * a_ppOutputStream = NULL)
{
   // First, try to find the asset file on disk.
   if (a_Asset.loadFromFile(a_Path))
   {
      return true;
   }

   // std::cout << "Failed to load asset from file.... Trying resouce packs" << std::endl;

   // We were not able to find the asset. Try one of the resource packs.
   std::shared_ptr<cPhysFsInputStream> l_pPhysFs =
      std::shared_ptr<cPhysFsInputStream>(new cPhysFsInputStream(a_Path));

   if (l_pPhysFs->isOpen())
   {
      // std::cout << "Success" << std::endl;
      if (a_ppOutputStream != NULL)
      {
         *a_ppOutputStream = l_pPhysFs;
      }

      return a_Asset.loadFromStream(*l_pPhysFs);
   }
   else
   {
      std::cout << "Error: Failed to load." << std::endl;
      return false;
   }
}

struct sRemovedObject
{
   cObject* m_pObject;
   bool m_DynamicAllocation;
};

// Class
class cResources
{
public:
   cResources();
   ~cResources();

   void CreateWindow(uint32_t a_Width, uint32_t a_Height, std::string a_Title);

   int32_t GenerateNewUniqueId();

   sf::RenderWindow* GetWindow();

   cView* GetView();
   void EnableView();

   cMessageDispatcher* GetMessageDispatcher();

   sf::Texture* LoadTexture(
      std::string a_TexturePath
      );

   std::map<std::string, cAnimation> GetCachedAnimationMap(
      std::string a_FileName
      );

   void CacheAnimationMap(
      std::string a_FileName,
      const std::map<std::string, cAnimation> & a_AnimationMap
      );

   std::shared_ptr<cVFileNode> LoadcVFile(
      std::string a_FileName
      );

   std::shared_ptr<sf::Font> LoadFont(
      std::string a_FileName
      );

   std::shared_ptr<cSound> LoadSoundBuffer(
      std::string a_FileName,
      uint32_t a_PoolSize = 1
      );

   void SetSoundVolume(double a_Volume);
   double GetSoundVolume();

   void PlayBackgroundMusic(std::string a_Path);
   sf::Music * GetBackGroundMusic();

   std::string GetActiveLevel();

   // TODO: This way of changing the level is really crappy
   void SetActiveLevel(std::string a_LevelName, bool a_Reload);
   bool ActiveLevelNeedsReload();

   // Used by the level manager when levels change
   void SetActiveLevelData(iLevelData* a_LevelData);

   void AddAssetResourcePack(std::string a_PackPath);

   // Used by objects to get at level data
   iLevelData* GetActiveLevelData();

   cVFileNode * GetGameConfigData();

   cEventTranslator * GetEventTranslator();

private:
   sf::RenderWindow m_Window;
   cView m_View;

   std::map<std::string, sf::Texture*> m_TextureMap;
   std::map<std::string, std::shared_ptr<cVFileNode>> m_SavedcVFiles;
   std::map<std::string, std::shared_ptr<sf::Font>> m_FontMap;

   // Filename,  <Animation name,Animation>
   std::map<std::string, std::map<std::string, cAnimation> > m_CachedAnimationTemplates;

   // Sounds
   std::map<std::string, std::shared_ptr<cSound>> m_SoundBufferMap;
   double m_SoundVolume;

   std::shared_ptr<sf::Music> m_BackgroundMusic;

   // This needs to be after the other resouces because it needs to be
   // destructed last.
   std::map<std::string, std::shared_ptr<cPhysFsInputStream>> m_StreamMap;

   // This container is meant to hold information that should persist for the
   // lifetime of the game.
   cVFileNode m_GameConfigData;

   // Keybindings
   cEventTranslator m_EventTranslator;

   // A different ID is given to each object.
   int32_t m_UniqueIdCount;

   cMessageDispatcher m_MessageDispatcher;

   // These things change when the level changes
   std::string m_ActiveLevel;
   bool m_ActiveLevelNeedsReload;

   iLevelData* m_pCurrentLevelData;

};

#endif
