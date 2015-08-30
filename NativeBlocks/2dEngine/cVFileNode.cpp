///////////////////////////////////////////////////////////////////////
//
// Includes
//
//////////////////////////////////////////////////////////////////////
#include "cVFileNode.h"

#include <iostream>
#include <fstream>

///////////////////////////////////////////////////////////////////////
//
// Static Helpers
//
//////////////////////////////////////////////////////////////////////

static std::vector<std::string> ExplodeString(std::string a_String, char a_Delim)
{
   std::vector<std::string> l_Result;
   std::string l_Part;
   for (std::string::iterator i = a_String.begin(); i != a_String.end(); ++i)
   {
      if (*i != a_Delim)
      {
         l_Part += *i;
      }
      else
      {
         // Remove leading and trailing whitespace
         size_t l_Start = l_Part.find_first_not_of(" \t\r\n");
         size_t l_End = l_Part.find_last_not_of(" \t\r\n");

         if (l_Start != std::string::npos && l_End != std::string::npos)
         {
            l_Part = l_Part.substr(l_Start, l_End + 1 - l_Start);
            l_Result.push_back(l_Part);
         }

         l_Part.clear();

      }
   }

   if (!l_Part.empty())
   {
      // Remove leading and trailing whitespace
      size_t l_Start = l_Part.find_first_not_of(" \t\r\n");
      size_t l_End = l_Part.find_last_not_of(" \t\r\n");

      if (l_Start != std::string::npos && l_End != std::string::npos)
      {
         l_Part = l_Part.substr(l_Start, l_End + 1 - l_Start);
         l_Result.push_back(l_Part);
      }
   }

   return l_Result;
}

///////////////////////////////////////////////////////////////////////
//
// Class Definition
//
//////////////////////////////////////////////////////////////////////
cVFileNode::cVFileNode()
   :m_Child(),
    m_Vector(),
    m_Value()
{
}

cVFileNode::~cVFileNode()
{
}

cVFileNode& cVFileNode::operator[](std::string a_Key)
{
   cVFileNode * l_pItem;

   // If this is an array but the user is trying to access a property in the
   // array, assume index 0.
   if (m_Vector.size() > 0)
   {
      l_pItem = &(m_Vector.begin()->m_Child[a_Key]);
   }
   else
   {
      l_pItem = &(m_Child[a_Key]);
   }

   if (l_pItem->m_Vector.size() == 0)
   {
      // This category didn't exist before, but we implicitly created it through
      // the map access. Need to create the first element in the category's
      // array.
      cVFileNode l_EmptyNode;
      l_pItem->m_Vector.push_back(l_EmptyNode);
   }

   return *(l_pItem);
}

cVFileNode& cVFileNode::operator[](int a_Index)
{
   if (static_cast<size_t>(a_Index + 1) > m_Vector.size())
   {
      m_Vector.resize(a_Index + 1);
   }
   return m_Vector[a_Index];
}

cVFileNode& cVFileNode::operator= (const std::string & a_rValue)
{
   m_Value = a_rValue;
   return *this;
}

cVFileNode::operator std::string() const
{
   return m_Value;
}

// TODO: Consider using more descriptive errors.
bool cVFileNode::LoadFile(std::string a_FileName)
{
   std::ifstream l_FileStream;
   l_FileStream.open(a_FileName.c_str());
   if (l_FileStream.fail())
   {
      l_FileStream.close();
      return false;
   }

   LoadStream(l_FileStream);
   l_FileStream.close();

   return true;
}

void cVFileNode::LoadStream(std::istream& a_rStream, cVFileNode * a_pRoot)
{
   if (a_pRoot == NULL)
   {
      a_pRoot = this;
   }

   cVFileNode * l_pLastCreatedNode = NULL;
   int32_t l_ArrayDepth = 0;

   // start reading the file line by line
   std::string l_Line;
   while (getline(a_rStream, l_Line))
   {
      std::vector<std::string> l_Explode;
      l_Explode = ExplodeString(l_Line, ':');

      if (l_Explode.size() == 0)
      {
         // This is a blank line
         continue;
      }
      else if (l_Explode.size() == 2)
      {
         // If there is something on the other side of the colon then this is just
         // a simple property.
         (*a_pRoot)[l_Explode[0]] = l_Explode[1];
         l_pLastCreatedNode = &(*a_pRoot)[l_Explode[0]];

         // Not in an array so reset the count
         l_ArrayDepth = 0;
      }
      else if (l_Explode.size() == 1)
      {
         if (l_Explode[0] == "{")
         {
            LoadStream(a_rStream, &(*l_pLastCreatedNode)[l_ArrayDepth]);
            ++l_ArrayDepth;
         }
         else if (l_Explode[0] == "}")
         {
            return;
         }
         else
         {
            // This will implicitly create a new category or empty property
            (*a_pRoot)[l_Explode[0]] = "";
            l_pLastCreatedNode = &(*a_pRoot)[l_Explode[0]];

            // Not in an array so reset the count
            l_ArrayDepth = 0;
         }
      }
   }
}

bool cVFileNode::ExportToFile(std::string a_FileName, std::string a_OutputGroup)
{
   // Can't have a vector without a label. Assume index 0 if so.
   cVFileNode * a_pRoot = this;
   if (m_Vector.size() != 0)
   {
      a_pRoot = &((*a_pRoot)[0]);
   }


   std::ofstream l_File;
   l_File.open(a_FileName.c_str());
   if (l_File.fail())
   {
      l_File.close();
      return false;
   }

   ExportToStream(l_File, a_pRoot);
   l_File.close();

   return true;
}

void cVFileNode::ExportToStream(
   std::ostream& a_rStream,
   cVFileNode * a_pRoot,
   uint32_t a_TabLevel
   )
{
   if (a_pRoot == NULL)
   {
      a_pRoot = this;
   }

   if (static_cast<std::string>(*a_pRoot).empty() == false)
   {
      a_rStream << static_cast<std::string>(*a_pRoot) << '\n';
   }

   // TODO: it would be nice if this function didn't rely on private stuff
   for (std::map<std::string, cVFileNode>::iterator i = a_pRoot->m_Child.begin();
      i != a_pRoot->m_Child.end();
      ++i
      )
   {
      a_rStream << _TabLevelToString(a_TabLevel) << i->first << ": ";
      if (static_cast<std::string>(i->second).empty())
      {
         // The child is starting a vector so we shouldn't expect anything after
         // the color. Print a new line ourselves.
         a_rStream << '\n';
      }

      ExportToStream(a_rStream, &(i->second), a_TabLevel);
   }
   for (std::vector<cVFileNode>::iterator i = a_pRoot->m_Vector.begin();
      i != a_pRoot->m_Vector.end();
      ++i
      )
   {
      if (i->Size() != 0)
      {
         a_rStream << _TabLevelToString(a_TabLevel) << "{\n";
         ExportToStream(a_rStream, &(*i), a_TabLevel + 1);
         a_rStream << _TabLevelToString(a_TabLevel) << "}\n";
      }
   }
}

std::string cVFileNode::GetProperty(std::string a_Property)
{
   if (m_Vector.size() > 0 && m_Vector.begin()->Size() != 0)
   {
      // This node contains a vector and the vector contains more than just a
      // placeholder node. Assume index 0.
      return m_Vector.begin()->m_Child[a_Property].m_Value;
   }
   else
   {
      return m_Child[a_Property].m_Value;
   }
}

void cVFileNode::SetProperty(std::string a_Property, std::string a_Value)
{
   if (m_Vector.size() > 0 && m_Vector.begin()->Size() != 0)
   {
      // This node contains a vector. Assume index 0.
      m_Vector.begin()->m_Child[a_Property].m_Value = a_Value;
   }
   else
   {
      m_Child[a_Property].m_Value = a_Value;
   }
}

size_t cVFileNode::Size() const
{
   if (m_Child.size() != 0)
   {
      return m_Child.size();
   }
   // m_Vector has a special case where there might be one empty object in it.
   // Treat this as size 0.
   else if (m_Vector.size() != 0 && !(m_Vector.size() == 1 && m_Vector.begin()->Size() == 0))
   {
      return m_Vector.size();
   }
   else if (m_Value.size() != 0)
   {
      return 1;
   }
   else
   {
      return 0;
   }
}

std::string cVFileNode::_TabLevelToString(uint32_t a_TabLevel)
{
   std::string l_ReturnString;
   for (uint32_t i = 0; i < a_TabLevel; ++i)
   {
      l_ReturnString += "   ";
   }
   return l_ReturnString;
}
