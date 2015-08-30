#include "cPhysFsInputStream.h"
#include <physfs.h>

cPhysFsInputStream::cPhysFsInputStream(std::string a_FileName)
   : m_pPhysFile(NULL)
{
   if (a_FileName.size() != 0)
   {
      _Open(a_FileName);
   }
}

cPhysFsInputStream::~cPhysFsInputStream()
{
   _Close();
}

void cPhysFsInputStream::Initialize()
{
   PHYSFS_init(NULL);
}

void cPhysFsInputStream::Deinit()
{
   PHYSFS_deinit();
}

void cPhysFsInputStream::AddAssetResourcePack(std::string a_Archive)
{
   if (a_Archive.size() != 0)
   {
      PHYSFS_addToSearchPath(a_Archive.c_str(), 1);
   }
}

bool cPhysFsInputStream::isOpen() const
{
   return (m_pPhysFile != NULL);
}

bool cPhysFsInputStream::_Open(std::string a_FileName)
{
   _Close();
   m_pPhysFile = PHYSFS_openRead(a_FileName.c_str());
   return isOpen();
}

void cPhysFsInputStream::_Close()
{
   if (isOpen())
   {
      PHYSFS_close(m_pPhysFile);
   }
   m_pPhysFile = NULL;
}

sf::Int64 cPhysFsInputStream::read(void* a_pData, sf::Int64 a_Size)
{
   if (!isOpen())
   {
      return -1;
   }

   // PHYSFS_read returns the number of 'objects' read or -1 on error.
   // We assume our objects are single bytes and request to read size
   // amount of them.
   return PHYSFS_read(
      m_pPhysFile,
      a_pData,
      1,
      static_cast<PHYSFS_uint32>(a_Size)
      );
}

sf::Int64 cPhysFsInputStream::seek(sf::Int64 a_Position)
{
   if (!isOpen())
   {
      return -1;
   }

  // PHYSFS_seek return 0 on error and non zero on success
   if (PHYSFS_seek(m_pPhysFile, a_Position))
   {
      return tell();
   }
   else
   {
      return -1;
   }
}

sf::Int64 cPhysFsInputStream::tell()
{
   if (!isOpen())
   {
      return -1;
   }

  // PHYSFS_tell returns the offset in bytes or -1 on error just like SFML
  // wants.
   return PHYSFS_tell(m_pPhysFile);
}

sf::Int64 cPhysFsInputStream::getSize()
{
   if (!isOpen())
   {
      return -1;
   }

   // PHYSFS_fileLength also the returns length of file or -1 on error just
   // like SFML wants.
   return PHYSFS_fileLength(m_pPhysFile);
}
