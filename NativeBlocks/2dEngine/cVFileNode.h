// This class is used to load Versitle Configuration files (VCf)
#ifndef ___cVFileNode_h___
#define ___cVFileNode_h___

#include <string>
#include <map>
#include <vector>
#include <stdint.h>

class cVFileNode
{
public:
   cVFileNode();
   ~cVFileNode();

   cVFileNode& operator[](std::string a_Key);
   cVFileNode& operator[](int a_Index);

   cVFileNode& operator= (const std::string & a_rValue);
   operator std::string() const;

   // TODO: Consider using exceptions instead of boolean status
   bool LoadFile(std::string a_File);
   void LoadStream(std::istream& a_rStream, cVFileNode * a_pRoot = NULL);

   bool ExportToFile(std::string a_FileName, std::string a_OutputGroup = "");
   void ExportToStream(
      std::ostream& a_rStream,
      cVFileNode * a_pRoot = NULL,
      uint32_t a_TabLevel = 0
      );


   std::string GetProperty(std::string a_Property);
   void SetProperty(std::string a_Property, std::string a_Value);

   // This function returns the size of whatever container has something
   size_t Size() const;

private:

   // Used when exporting to stream/file
   std::string _TabLevelToString(uint32_t a_TabLevel);

   // This map will contain nodes that only have a value or nodes that have
   // vectors of other nodes.
   std::map<std::string, cVFileNode> m_Child;

   // All of the nodes in the vector will have a "child" map
   std::vector<cVFileNode> m_Vector;

   std::string m_Value;
};

#endif
