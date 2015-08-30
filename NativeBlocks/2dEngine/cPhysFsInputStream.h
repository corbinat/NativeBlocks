#ifndef ___cPhysFsInputStream_h___
#define ___cPhysFsInputStream_h___

#include <iostream>
#include <physfs.h>
#include <SFML/Graphics.hpp>

class cPhysFsInputStream : public sf::InputStream
{
public:

   cPhysFsInputStream(std::string a_FileName);

   virtual ~cPhysFsInputStream();

   static void Initialize();

   static void AddAssetResourcePack(std::string a_Archive);

   static void Deinit();

   bool isOpen() const;

   virtual sf::Int64 read(void* a_pData, sf::Int64 a_Size);

   virtual sf::Int64 seek(sf::Int64 a_Position);

   virtual sf::Int64 tell();

   virtual sf::Int64 getSize();

private:

   bool _Open(std::string a_FileName);
   void _Close();

   PHYSFS_File* m_pPhysFile;

};

#endif
