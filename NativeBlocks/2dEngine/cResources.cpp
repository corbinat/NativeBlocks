#include "cResources.h"
#include "cVFileNode.h"
#include "cSound.h"

#include "SFML/Audio.hpp"

#include <sstream>

cResources::cResources()
   : m_Window(),
     m_View(),
     m_TextureMap(),
     m_SavedcVFiles(),
     m_FontMap(),
     m_CachedAnimationTemplates(),
     m_SoundBufferMap(),
     m_SoundVolume(100),
     m_BackgroundMusic(),
     m_StreamMap(),
     m_GameConfigData(),
     m_EventTranslator(),
     m_UniqueIdCount(0),
     m_MessageDispatcher(m_UniqueIdCount++),
     m_ActiveLevel(),
     m_ActiveLevelNeedsReload(false),
     m_pCurrentLevelData(NULL)
{
   // Turn off SFML error prints.
   // TODO: Redirect to file for debug builds
   sf::err().rdbuf(NULL);

   // Initialize PhysFS
   cPhysFsInputStream::Initialize();

   // This kind of sucks. Ideally this wouldn't need to be a pointer at all.
   // However, I need to be able to call the destructor before calling Deinit
   // on the PhysFs stream.
   m_BackgroundMusic = std::shared_ptr<sf::Music>(new sf::Music());
}

cResources::~cResources()
{
   // Destroy all the textures we made
   std::map<std::string,sf::Texture*>::iterator l_It;
   for (l_It = m_TextureMap.begin(); l_It != m_TextureMap.end(); ++l_It)
   {
      delete l_It->second;
   }

   // Stop da music and clean up so we don't segfault when the PhysFs stream
   // goes away.
   m_BackgroundMusic->stop();
   m_BackgroundMusic.reset();

   // Manually close all cPhysFsInputStream before the Deinit
   std::map<std::string, std::shared_ptr<cPhysFsInputStream>>::iterator l_It2;
   for (l_It2 = m_StreamMap.begin(); l_It2 != m_StreamMap.end(); ++l_It2)
   {
      l_It2->second.reset();
   }

   cPhysFsInputStream::Deinit();
}

void cResources::CreateWindow(
   uint32_t a_Width,
   uint32_t a_Height,
   std::string a_Title
   )
{
   //DebugTrace();
   m_Window.create(
      sf::VideoMode(a_Width, a_Height),
      a_Title,
      sf::Style::Titlebar | sf::Style::Close
      );
   m_Window.setVerticalSyncEnabled(false);

   // We don't want repeated key press events when the key is held down.
   m_Window.setKeyRepeatEnabled(false);

   // Don't burn up the cpu
   m_Window.setFramerateLimit(300);
}

int32_t cResources::GenerateNewUniqueId()
{
   return ++m_UniqueIdCount;
}

sf::RenderWindow* cResources::GetWindow()
{
   return &m_Window;
}

cView* cResources::GetView()
{
   return &m_View;
}

void cResources::EnableView()
{
   m_Window.setView(m_View);
}

cMessageDispatcher* cResources::GetMessageDispatcher()
{
   return &m_MessageDispatcher;
}

sf::Texture* cResources::LoadTexture(
   std::string a_TexturePath
   )
{
   // Check to see if we already have this texture. We don't want to load the
   // same one more than once.
   // TODO: This can be improved. Create a struct that also stores how many
   // objects are using the textures. That way we can unload them when done.

   std::map<std::string, sf::Texture*>::iterator l_Iterator
      = m_TextureMap.find(a_TexturePath);

   if (l_Iterator != m_TextureMap.end())
   {
      return l_Iterator->second;
   }

   // We don't already have this texture so we need to load it.
   sf::Texture* l_pTexture = new (std::nothrow) sf::Texture;
   if (l_pTexture == NULL)
   {
      return NULL;
   }
   bool l_Success = LoadAsset<sf::Texture>(*l_pTexture, a_TexturePath);

   if (!l_Success)
   {
      DebugPrintf("Failed to load texture from file\n");
      delete l_pTexture;
      return NULL;
   }

   m_TextureMap[a_TexturePath] = l_pTexture;
   return l_pTexture;
}

std::map<std::string, cAnimation> cResources::GetCachedAnimationMap(
   std::string a_FileName
   )
{
   auto l_Iterator = m_CachedAnimationTemplates.find(a_FileName);

   if (l_Iterator != m_CachedAnimationTemplates.end())
   {
      return l_Iterator->second;
   }

   // Didn't have one cached
   return std::map<std::string, cAnimation>();
}

void cResources::CacheAnimationMap(
   std::string a_FileName,
   const std::map<std::string, cAnimation> & a_AnimationMap
   )
{
   auto l_Iterator = m_CachedAnimationTemplates.find(a_FileName);

   if (l_Iterator != m_CachedAnimationTemplates.end())
   {
      // This one is already cached.
      return;
   }

   // Didn't have one cached
   m_CachedAnimationTemplates[a_FileName] = a_AnimationMap;
   return;
}

std::shared_ptr<cVFileNode> cResources::LoadcVFile(
   std::string a_FileName
   )
{
   auto l_Iterator = m_SavedcVFiles.find(a_FileName);

   if (l_Iterator != m_SavedcVFiles.end())
   {
      return l_Iterator->second;
   }

   std::shared_ptr<cVFileNode> l_pFile(new cVFileNode);

   // First, try to find the asset file on disk. Not using the LoadAsset function
   // because cVFileNode doesn't follow the SFML style of loading from file
   bool l_LoadSuccess = l_pFile->LoadFile(a_FileName);

   if (!l_LoadSuccess)
   {
      // We were not able to find the asset. Try one of the resource packs.
      cPhysFsInputStream l_PhysFs(a_FileName);
      if (l_PhysFs.isOpen())
      {
         char * l_Data = new char[l_PhysFs.getSize()];
         l_PhysFs.read(l_Data, l_PhysFs.getSize());
         std::string l_String(l_Data, l_PhysFs.getSize());
         std::istringstream l_Stream(l_String);
         delete[] l_Data;

         // TODO: Somehow get errors
         l_pFile->LoadStream(l_Stream);
      }
   }

   //~ if (!l_LoadSuccess)
   //~ {
      //~ DebugPrintf("ERROR: Failed to load animation file\n");
      //~ return std::shared_ptr<cVFileNode>();
   //~ }

   m_SavedcVFiles[a_FileName] = l_pFile;
   return l_pFile;
}


std::shared_ptr<sf::Font> cResources::LoadFont(
   std::string a_FileName
   )
{
   auto l_Iterator = m_FontMap.find(a_FileName);

   if (l_Iterator != m_FontMap.end())
   {
      return l_Iterator->second;
   }

   std::shared_ptr<sf::Font> l_pFont(new sf::Font);
   std::shared_ptr<cPhysFsInputStream> l_pOutputStream = NULL;
   bool l_LoadSuccess = LoadAsset<sf::Font>(*l_pFont, a_FileName, &l_pOutputStream);

   if (!l_LoadSuccess)
   {
      std::cout << "Failed to load fonts" << std::endl;
      DebugPrintf("ERROR: Failed to load font\n");
      return std::shared_ptr<sf::Font>();
   }

   //std::cout << "success" << std::endl;
   m_FontMap[a_FileName] = l_pFont;

   // Font is a special case where the stream has to stay alive.
   if (l_pOutputStream.get() != NULL)
   {
      m_StreamMap[a_FileName] = l_pOutputStream;
   }
   return l_pFont;
}

std::shared_ptr<cSound> cResources::LoadSoundBuffer(
   std::string a_SoundBufferPath,
   uint32_t a_PoolSize
   )
{
   // Check to see if we already have this SoundBuffer. We don't want to load the
   // same one more than once.
   // TODO: This can be improved. Create a struct that also stores how many
   // objects are using the SoundBuffer. That way we can unload them when done.

   std::map<std::string, std::shared_ptr<cSound>>::iterator l_Iterator
      = m_SoundBufferMap.find(a_SoundBufferPath);

   if (l_Iterator != m_SoundBufferMap.end())
   {
      return l_Iterator->second;
   }

   // We don't already have this texture so we need to load it.
   std::shared_ptr<sf::SoundBuffer> l_pSoundBuffer(new sf::SoundBuffer);
   bool l_Success = LoadAsset<sf::SoundBuffer>(*l_pSoundBuffer, a_SoundBufferPath);

   if (!l_Success)
   {
      return std::shared_ptr<cSound>();
   }

   std::shared_ptr<cSound> l_pNewSound(new cSound);
   l_pNewSound->SetBuffer(l_pSoundBuffer);
   l_pNewSound->SetPoolSize(a_PoolSize);
   l_pNewSound->SetVolume(m_SoundVolume);

   m_SoundBufferMap[a_SoundBufferPath] = l_pNewSound;
   return l_pNewSound;
}

void cResources::SetSoundVolume(double a_Volume)
{
   m_SoundVolume = a_Volume;
   for (auto& i : m_SoundBufferMap)
   {
      i.second->SetVolume(a_Volume);
   }
}

double cResources::GetSoundVolume()
{
   return m_SoundVolume;
}

void cResources::PlayBackgroundMusic(std::string a_Path)
{
   // Can't use LoadAsset because the function is openFromFile instead of load
   // from file
   if (m_BackgroundMusic->openFromFile(a_Path))
   {
      return;
   }

   // See if the background music exists as a stream
   auto l_Iterator = m_StreamMap.find(a_Path);
   if (l_Iterator != m_StreamMap.end())
   {
      m_BackgroundMusic->openFromStream(*(l_Iterator->second));
      return;
   }

   // We were not able to find the asset. Try one of the resource packs.
   std::shared_ptr<cPhysFsInputStream> l_pPhysFs =
      std::shared_ptr<cPhysFsInputStream>(new cPhysFsInputStream(a_Path));

   if (l_pPhysFs->isOpen())
   {
      m_StreamMap[a_Path] = l_pPhysFs;
      m_BackgroundMusic->openFromStream(*l_pPhysFs);
      return;
   }
   else
   {
      std::cout << "Error: Failed to load background music." << std::endl;
   }

}

sf::Music * cResources::GetBackGroundMusic()
{
   return m_BackgroundMusic.get();
}

std::string cResources::GetActiveLevel()
{
   return m_ActiveLevel;
}

void cResources::SetActiveLevel(std::string a_LevelName, bool a_Reload)
{
   m_ActiveLevel = a_LevelName;
   m_ActiveLevelNeedsReload = a_Reload;
}

// TODO: This is kind of crappy
bool cResources::ActiveLevelNeedsReload()
{
   bool l_Return = m_ActiveLevelNeedsReload;
   m_ActiveLevelNeedsReload = false;
   return l_Return;
}

void cResources::SetActiveLevelData(iLevelData* a_LevelData)
{
   m_pCurrentLevelData = a_LevelData;
}

void cResources::AddAssetResourcePack(std::string a_PackPath)
{
   cPhysFsInputStream::AddAssetResourcePack(a_PackPath);
}


iLevelData* cResources::GetActiveLevelData()
{
   return m_pCurrentLevelData;
}

cVFileNode * cResources::GetGameConfigData()
{
   return &m_GameConfigData;
}

cEventTranslator * cResources::GetEventTranslator()
{
   return &m_EventTranslator;
}
